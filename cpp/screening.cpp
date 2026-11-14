#include "screening.h"
#include "statistics.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

static inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static inline std::string escapeJSON(const std::string& s) {
    std::ostringstream o;
    for (char c : s) {
        if (c == '"') o << "\\\"";
        else if (c == '\\') o << "\\\\";
        else if (c == '\b') o << "\\b";
        else if (c == '\f') o << "\\f";
        else if (c == '\n') o << "\\n";
        else if (c == '\r') o << "\\r";
        else if (c == '\t') o << "\\t";
        else o << c;
    }
    return o.str();
}

ScreeningPipeline::ScreeningPipeline(double lLimit, double iLimit, double zThresh)
    : leakageLimit(lLimit), iddqLimit(iLimit), zThreshold(zThresh),
      passCount(0), monitorCount(0), suspectCount(0), rejectCount(0) {}

ScreeningPipeline::~ScreeningPipeline() {
    clear();
}

void ScreeningPipeline::setLimits(double lLimit, double iLimit, double zThresh) {
    leakageLimit = lLimit;
    iddqLimit = iLimit;
    zThreshold = zThresh;
}

void ScreeningPipeline::clear() {
    for (Component* comp : allComponents) {
        delete comp;
    }
    allComponents.clear();
    lotDirectory.clear();
    hashTable.clear();
    bst.clear();
    maxHeap.clear();
    lotStatsMap.clear();

    passCount = monitorCount = suspectCount = rejectCount = 0;
}

bool ScreeningPipeline::loadFromCSV(const std::string& filePath, std::string& errorMsg) {
    clear();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        errorMsg = "Unable to open file: " + filePath;
        return false;
    }

    std::string line;
    if (!std::getline(file, line)) {
        errorMsg = "CSV file is empty: " + filePath;
        return false;
    }

    // Parse header
    std::vector<std::string> headers;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        headers.push_back(trim(token));
    }

    int idCol = 0, lotCol = 1;
    int l0Col = 2, l24Col = 3;
    int i0Col = 4, i24Col = 5;

    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        std::transform(h.begin(), h.end(), h.begin(), ::tolower);
        if (h.find("component") != std::string::npos || h == "id") { idCol = (int)i; }
        else if (h.find("lot") != std::string::npos) { lotCol = (int)i; }
        else if (h.find("leakage") != std::string::npos && (h.find("0") != std::string::npos || h.find("t0") != std::string::npos)) { l0Col = (int)i; }
        else if (h.find("leakage") != std::string::npos && (h.find("24") != std::string::npos || h.find("t24") != std::string::npos)) { l24Col = (int)i; }
        else if (h.find("iddq") != std::string::npos && (h.find("0") != std::string::npos || h.find("t0") != std::string::npos)) { i0Col = (int)i; }
        else if (h.find("iddq") != std::string::npos && (h.find("24") != std::string::npos || h.find("t24") != std::string::npos)) { i24Col = (int)i; }
    }

    int rowNum = 1;
    while (std::getline(file, line)) {
        rowNum++;
        line = trim(line);
        if (line.empty()) continue;

        std::stringstream lineStream(line);
        std::vector<std::string> cells;
        while (std::getline(lineStream, token, ',')) {
            cells.push_back(trim(token));
        }

        if (cells.size() < 6) {
            continue; // Skip malformed rows
        }

        std::string compID = (idCol < (int)cells.size()) ? cells[idCol] : ("C" + std::to_string(rowNum));
        std::string lotID  = (lotCol < (int)cells.size()) ? cells[lotCol] : "LOT-UNKNOWN";

        double l0 = 0.0, l24 = 0.0, i0 = 0.0, i24 = 0.0;
        try {
            if (l0Col < (int)cells.size()) l0 = std::stod(cells[l0Col]);
            if (l24Col < (int)cells.size()) l24 = std::stod(cells[l24Col]);
            if (i0Col < (int)cells.size()) i0 = std::stod(cells[i0Col]);
            if (i24Col < (int)cells.size()) i24 = std::stod(cells[i24Col]);
        } catch (...) {
            continue; // Skip non-numeric measurement row
        }

        // Allocate Component and store sequential arrays: leakage[3], iddq[3]
        Component* comp = new Component(compID, lotID, l0, l24, i0, i24);
        allComponents.push_back(comp);

        // Dynamic Linked List: Add component to its manufacturing Lot list
        LotLinkedList* lotList = lotDirectory.getOrCreateLot(lotID);
        lotList->insert(comp);
    }

    if (allComponents.empty()) {
        errorMsg = "No valid component records found in CSV.";
        return false;
    }

    // Execute the complete screening pipeline across all Data Structures
    runScreening();
    return true;
}

void ScreeningPipeline::runScreening() {
    passCount = monitorCount = suspectCount = rejectCount = 0;
    lotStatsMap.clear();

    // Iterate through each Lot in the dynamic Lot Linked List
    LotNode* lotNode = lotDirectory.getHead();
    while (lotNode) {
        std::string lotID = lotNode->lotID;
        LotLinkedList& lotList = lotNode->componentList;
        int n = lotList.size();

        if (n == 0) {
            lotNode = lotNode->next;
            continue;
        }

        std::vector<Component*> comps = lotList.toVector();

        // Prepare arrays for Merge Sort & Median/MAD calculation
        std::vector<double> arrL0(n), arrL24(n), arrL168(n), arrLDrift(n);
        std::vector<double> arrI0(n), arrI24(n), arrI168(n), arrIDrift(n);

        for (int i = 0; i < n; ++i) {
            arrL0[i] = comps[i]->leakage[0];
            arrL24[i] = comps[i]->leakage[1];
            arrL168[i] = comps[i]->leakage[2];
            arrLDrift[i] = comps[i]->leakageDrift;

            arrI0[i] = comps[i]->iddq[0];
            arrI24[i] = comps[i]->iddq[1];
            arrI168[i] = comps[i]->iddq[2];
            arrIDrift[i] = comps[i]->iddqDrift;
        }

        // Calculate Lot Statistics using Merge Sort
        LotStatistics stats;
        stats.lotID = lotID;
        stats.count = n;

        stats.medianLeakage0h = Statistics::calculateMedian(arrL0.data(), n);
        stats.madLeakage0h    = Statistics::calculateMAD(arrL0.data(), n, stats.medianLeakage0h);
        stats.medianLeakage24h = Statistics::calculateMedian(arrL24.data(), n);
        stats.madLeakage24h    = Statistics::calculateMAD(arrL24.data(), n, stats.medianLeakage24h);
        stats.medianLeakage168hPred = Statistics::calculateMedian(arrL168.data(), n);
        stats.madLeakage168hPred    = Statistics::calculateMAD(arrL168.data(), n, stats.medianLeakage168hPred);

        stats.medianIddq0h = Statistics::calculateMedian(arrI0.data(), n);
        stats.madIddq0h    = Statistics::calculateMAD(arrI0.data(), n, stats.medianIddq0h);
        stats.medianIddq24h = Statistics::calculateMedian(arrI24.data(), n);
        stats.madIddq24h    = Statistics::calculateMAD(arrI24.data(), n, stats.medianIddq24h);
        stats.medianIddq168hPred = Statistics::calculateMedian(arrI168.data(), n);
        stats.madIddq168hPred    = Statistics::calculateMAD(arrI168.data(), n, stats.medianIddq168hPred);

        stats.medianLeakageDrift = Statistics::calculateMedian(arrLDrift.data(), n);
        stats.madLeakageDrift    = Statistics::calculateMAD(arrLDrift.data(), n, stats.medianLeakageDrift);
        stats.medianIddqDrift = Statistics::calculateMedian(arrIDrift.data(), n);
        stats.madIddqDrift    = Statistics::calculateMAD(arrIDrift.data(), n, stats.medianIddqDrift);

        lotStatsMap[lotID] = stats;

        // Perform Component-Level Analysis
        for (int i = 0; i < n; ++i) {
            Component* c = comps[i];

            // 1. Position Analysis (Robust Z-score relative to Lot population)
            c->leakagePosZ = Statistics::calculateRobustZ(c->leakage[1], stats.medianLeakage24h, stats.madLeakage24h);
            c->iddqPosZ    = Statistics::calculateRobustZ(c->iddq[1], stats.medianIddq24h, stats.madIddq24h);
            c->positionScore = std::max(c->leakagePosZ, c->iddqPosZ);

            // 2. Drift Analysis (Degradation relative to Lot Drift)
            c->leakageDriftZ = Statistics::calculateRobustZ(c->leakageDrift, stats.medianLeakageDrift, stats.madLeakageDrift);
            c->iddqDriftZ    = Statistics::calculateRobustZ(c->iddqDrift, stats.medianIddqDrift, stats.madIddqDrift);
            c->driftScore    = std::max(c->leakageDriftZ, c->iddqDriftZ);

            // 3. 168h Forecast Risk
            // Evaluates how close or past the engineering limits the 168h prediction lands
            double leakRatio = c->leakage[2] / leakageLimit;
            double iddqRatio = c->iddq[2] / iddqLimit;
            double maxRatio = std::max(leakRatio, iddqRatio);
            if (maxRatio >= 1.0) {
                c->forecastRisk = 3.0 + (maxRatio - 1.0) * 5.0; // Exceeds limit by 168h
            } else if (maxRatio >= 0.85) {
                c->forecastRisk = 1.5 + (maxRatio - 0.85) * 10.0; // Approaching limit
            } else {
                c->forecastRisk = std::max(0.0, (maxRatio - 0.5) * 2.0);
            }

            // 4. Deterministic Engineering Limit Gate (Higher authority than statistics)
            // If measured 0h or 24h exceeds limit -> REJECT
            if (c->leakage[0] >= leakageLimit || c->leakage[1] >= leakageLimit ||
                c->iddq[0] >= iddqLimit || c->iddq[1] >= iddqLimit) 
            {
                c->limitViolated = true;
                c->decision = "REJECT";
                c->reason = "Engineering limit violated on measured timepoint(s) (deterministic rule)";
                // Risk Score gives top priority in Max Heap
                double excess = std::max(c->leakage[1] - leakageLimit, c->iddq[1] - iddqLimit);
                c->riskScore = 15.0 + std::max(0.0, excess);
                rejectCount++;
            } 
            else 
            {
                c->limitViolated = false;

                // 5. Evidence-based abnormal indicator tallying
                int abnormalCount = 0;
                std::string reasons = "";

                if (c->positionScore >= zThreshold) {
                    abnormalCount++;
                    reasons += "Unusual lot position (Z=" + std::to_string(std::round(c->positionScore * 10.0) / 10.0).substr(0, 4) + "); ";
                }
                if (c->driftScore >= zThreshold) {
                    abnormalCount++;
                    reasons += "Elevated burn-in drift (Z=" + std::to_string(std::round(c->driftScore * 10.0) / 10.0).substr(0, 4) + "); ";
                }
                if (c->leakage[2] >= leakageLimit || c->iddq[2] >= iddqLimit || c->forecastRisk >= 2.0) {
                    abnormalCount++;
                    reasons += "Forecast trajectory approaching/exceeding limit at 168h; ";
                }

                c->abnormalIndicatorsCount = abnormalCount;

                // Combined Latent-Defect Risk Score
                c->riskScore = c->positionScore + c->driftScore + c->forecastRisk;

                // Decision Assignment (Section 15)
                if (abnormalCount == 0) {
                    c->decision = "PASS";
                    c->reason = "Nominal performance: measurements, drift, and 168h projection within expected lot boundaries.";
                    passCount++;
                } else if (abnormalCount == 1) {
                    c->decision = "MONITOR";
                    c->reason = "Single abnormal indicator: " + reasons + "recommended for secondary observation.";
                    monitorCount++;
                } else {
                    c->decision = "SUSPECT";
                    c->reason = "Multiple abnormal indicators: " + reasons + "requires engineering triage.";
                    suspectCount++;
                }
            }

            // Populate Core Data Structures:
            // 4. Hash Table (O(1) direct lookup by ID)
            hashTable.insert(c->componentID, c);

            // 5. Binary Search Tree (O(log n) ordered storage)
            bst.insert(c);

            // 6. Max Heap (Priority queue for latent defect risk prioritization)
            maxHeap.insert(c);
        }

        lotNode = lotNode->next;
    }
}

std::string ScreeningPipeline::exportJSON() const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);

    json << "{\n";
    json << "  \"metadata\": {\n";
    json << "    \"totalComponents\": " << allComponents.size() << ",\n";
    json << "    \"lotCount\": " << lotStatsMap.size() << ",\n";
    json << "    \"leakageLimit\": " << leakageLimit << ",\n";
    json << "    \"iddqLimit\": " << iddqLimit << ",\n";
    json << "    \"zThreshold\": " << zThreshold << "\n";
    json << "  },\n";

    json << "  \"kpi\": {\n";
    json << "    \"pass\": " << passCount << ",\n";
    json << "    \"monitor\": " << monitorCount << ",\n";
    json << "    \"suspect\": " << suspectCount << ",\n";
    json << "    \"reject\": " << rejectCount << ",\n";
    double yield = allComponents.empty() ? 0.0 : ((double)passCount / allComponents.size()) * 100.0;
    json << "    \"passYield\": " << yield << "\n";
    json << "  },\n";

    // Lot Statistics
    json << "  \"lots\": [\n";
    bool firstLot = true;
    for (auto const& [lotID, stats] : lotStatsMap) {
        if (!firstLot) json << ",\n";
        firstLot = false;
        json << "    {\n";
        json << "      \"lotID\": \"" << escapeJSON(stats.lotID) << "\",\n";
        json << "      \"count\": " << stats.count << ",\n";
        json << "      \"medianLeakage0h\": " << stats.medianLeakage0h << ",\n";
        json << "      \"madLeakage0h\": " << stats.madLeakage0h << ",\n";
        json << "      \"medianLeakage24h\": " << stats.medianLeakage24h << ",\n";
        json << "      \"madLeakage24h\": " << stats.madLeakage24h << ",\n";
        json << "      \"medianLeakage168hPred\": " << stats.medianLeakage168hPred << ",\n";
        json << "      \"madLeakage168hPred\": " << stats.madLeakage168hPred << ",\n";
        json << "      \"medianIddq0h\": " << stats.medianIddq0h << ",\n";
        json << "      \"madIddq0h\": " << stats.madIddq0h << ",\n";
        json << "      \"medianIddq24h\": " << stats.medianIddq24h << ",\n";
        json << "      \"madIddq24h\": " << stats.madIddq24h << ",\n";
        json << "      \"medianIddq168hPred\": " << stats.medianIddq168hPred << ",\n";
        json << "      \"madIddq168hPred\": " << stats.madIddq168hPred << ",\n";
        json << "      \"medianLeakageDrift\": " << stats.medianLeakageDrift << ",\n";
        json << "      \"madLeakageDrift\": " << stats.madLeakageDrift << ",\n";
        json << "      \"medianIddqDrift\": " << stats.medianIddqDrift << ",\n";
        json << "      \"madIddqDrift\": " << stats.madIddqDrift << "\n";
        json << "    }";
    }
    json << "\n  ],\n";

    // Top Risk Components from Max Heap
    std::vector<Component*> topRisk = maxHeap.getTopK(25);
    json << "  \"topRiskComponents\": [\n";
    for (size_t i = 0; i < topRisk.size(); ++i) {
        if (i > 0) json << ",\n";
        Component* c = topRisk[i];
        json << "    {\n";
        json << "      \"rank\": " << (i + 1) << ",\n";
        json << "      \"componentID\": \"" << escapeJSON(c->componentID) << "\",\n";
        json << "      \"lotID\": \"" << escapeJSON(c->lotID) << "\",\n";
        json << "      \"riskScore\": " << c->riskScore << ",\n";
        json << "      \"decision\": \"" << escapeJSON(c->decision) << "\",\n";
        json << "      \"positionScore\": " << c->positionScore << ",\n";
        json << "      \"driftScore\": " << c->driftScore << ",\n";
        json << "      \"forecastRisk\": " << c->forecastRisk << ",\n";
        json << "      \"leakage0h\": " << c->leakage[0] << ",\n";
        json << "      \"leakage24h\": " << c->leakage[1] << ",\n";
        json << "      \"leakage168h\": " << c->leakage[2] << ",\n";
        json << "      \"iddq0h\": " << c->iddq[0] << ",\n";
        json << "      \"iddq24h\": " << c->iddq[1] << ",\n";
        json << "      \"iddq168h\": " << c->iddq[2] << ",\n";
        json << "      \"reason\": \"" << escapeJSON(c->reason) << "\"\n";
        json << "    }";
    }
    json << "\n  ],\n";

    // Data Structures Diagnostic & Visualizer Payload
    json << "  \"dataStructures\": {\n";
    json << "    \"hashTable\": {\n";
    json << "      \"capacity\": " << hashTable.getTableCapacity() << ",\n";
    json << "      \"elementCount\": " << hashTable.size() << ",\n";
    json << "      \"occupiedBuckets\": " << hashTable.getOccupiedBucketCount() << ",\n";
    json << "      \"collisions\": " << hashTable.getCollisionCount() << ",\n";
    json << "      \"maxChainLength\": " << hashTable.getMaxChainLength() << ",\n";
    json << "      \"loadFactor\": " << hashTable.getLoadFactor() << ",\n";
    json << "      \"activeBucketsSample\": [\n";
    auto activeBuckets = hashTable.getActiveBuckets(30);
    for (size_t b = 0; b < activeBuckets.size(); ++b) {
        if (b > 0) json << ",\n";
        json << "        {\"index\": " << activeBuckets[b].bucketIndex << ", \"chain\": [";
        for (size_t k = 0; k < activeBuckets[b].keys.size(); ++k) {
            if (k > 0) json << ", ";
            json << "\"" << escapeJSON(activeBuckets[b].keys[k]) << "\"";
        }
        json << "]}";
    }
    json << "\n      ]\n";
    json << "    },\n";

    json << "    \"bst\": {\n";
    json << "      \"nodeCount\": " << bst.size() << ",\n";
    json << "      \"treeHeight\": " << bst.getHeight() << ",\n";
    json << "      \"isBalanced\": " << (bst.getHeight() <= 2.5 * std::log2(std::max(2, bst.size())) ? "true" : "false") << "\n";
    json << "    },\n";

    json << "    \"maxHeap\": {\n";
    json << "      \"totalItems\": " << maxHeap.size() << ",\n";
    json << "      \"peekMaxRisk\": " << (maxHeap.peekMax() ? maxHeap.peekMax()->riskScore : 0.0) << ",\n";
    json << "      \"peekMaxID\": \"" << (maxHeap.peekMax() ? escapeJSON(maxHeap.peekMax()->componentID) : "") << "\",\n";
    json << "      \"binaryTreeNodes\": [\n";
    auto heapNodes = maxHeap.getHeapStructure(31);
    for (size_t h = 0; h < heapNodes.size(); ++h) {
        if (h > 0) json << ",\n";
        json << "        {\n";
        json << "          \"index\": " << heapNodes[h].index << ",\n";
        json << "          \"id\": \"" << escapeJSON(heapNodes[h].componentID) << "\",\n";
        json << "          \"lot\": \"" << escapeJSON(heapNodes[h].lotID) << "\",\n";
        json << "          \"risk\": " << heapNodes[h].riskScore << ",\n";
        json << "          \"decision\": \"" << escapeJSON(heapNodes[h].decision) << "\",\n";
        json << "          \"parent\": " << heapNodes[h].parentIndex << ",\n";
        json << "          \"left\": " << heapNodes[h].leftIndex << ",\n";
        json << "          \"right\": " << heapNodes[h].rightIndex << "\n";
        json << "        }";
    }
    json << "\n      ]\n";
    json << "    }\n";
    json << "  },\n";

    // All Components list for searching & tabular view
    json << "  \"components\": [\n";
    for (size_t i = 0; i < allComponents.size(); ++i) {
        if (i > 0) json << ",\n";
        Component* c = allComponents[i];
        json << "    {\n";
        json << "      \"id\": \"" << escapeJSON(c->componentID) << "\",\n";
        json << "      \"lot\": \"" << escapeJSON(c->lotID) << "\",\n";
        json << "      \"leakage\": [" << c->leakage[0] << ", " << c->leakage[1] << ", " << c->leakage[2] << "],\n";
        json << "      \"iddq\": [" << c->iddq[0] << ", " << c->iddq[1] << ", " << c->iddq[2] << "],\n";
        json << "      \"leakageDrift\": " << c->leakageDrift << ",\n";
        json << "      \"iddqDrift\": " << c->iddqDrift << ",\n";
        json << "      \"positionScore\": " << c->positionScore << ",\n";
        json << "      \"driftScore\": " << c->driftScore << ",\n";
        json << "      \"forecastRisk\": " << c->forecastRisk << ",\n";
        json << "      \"riskScore\": " << c->riskScore << ",\n";
        json << "      \"decision\": \"" << escapeJSON(c->decision) << "\",\n";
        json << "      \"reason\": \"" << escapeJSON(c->reason) << "\"\n";
        json << "    }";
    }
    json << "\n  ]\n";
    json << "}\n";

    return json.str();
}
