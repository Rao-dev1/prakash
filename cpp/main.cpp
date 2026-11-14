#include "screening.h"
#include "tests.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

void displayBanner() {
    std::cout << "\n";
    std::cout << "========================================================\n";
    std::cout << "                       PRAKASH-DS\n";
    std::cout << "     Burn-In Component Screening & Prioritization System\n";
    std::cout << "         (Data Structures & Algorithms Course Project)\n";
    std::cout << "========================================================\n";
}

void printComponentCard(const Component* c) {
    if (!c) {
        std::cout << "\n[-] Component not found.\n";
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "          COMPONENT ANALYSIS\n";
    std::cout << "========================================\n";
    std::cout << "Component ID : " << c->componentID << "\n";
    std::cout << "Lot ID       : " << c->lotID << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "LEAKAGE CURRENT (uA):\n";
    std::cout << "  0h         : " << c->leakage[0] << "\n";
    std::cout << "  24h        : " << c->leakage[1] << "\n";
    std::cout << "  168h (Pred): " << c->leakage[2] << "\n";
    std::cout << "  Drift      : " << c->leakageDrift << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "IDDQ CURRENT (uA):\n";
    std::cout << "  0h         : " << c->iddq[0] << "\n";
    std::cout << "  24h        : " << c->iddq[1] << "\n";
    std::cout << "  168h (Pred): " << c->iddq[2] << "\n";
    std::cout << "  Drift      : " << c->iddqDrift << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "STATISTICAL METRICS:\n";
    std::cout << "  Position Z : " << c->positionScore << " (Leakage: " << c->leakagePosZ << ", IDDQ: " << c->iddqPosZ << ")\n";
    std::cout << "  Drift Z    : " << c->driftScore << " (Leakage: " << c->leakageDriftZ << ", IDDQ: " << c->iddqDriftZ << ")\n";
    std::cout << "  Forecast   : " << c->forecastRisk << "\n";
    std::cout << "  Risk Score : " << c->riskScore << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "SCREENING DECISION: [" << c->decision << "]\n";
    std::cout << "Reason       : " << c->reason << "\n";
    std::cout << "========================================\n";
}

int main(int argc, char* argv[]) {
    // Section 34 Automated Verification Test Mode
    if (argc >= 2 && std::string(argv[1]) == "--test") {
        bool allPassed = Tests::runAllTests(true);
        return allPassed ? 0 : 1;
    }
    if (argc >= 2 && std::string(argv[1]) == "--test-json") {
        std::cout << Tests::runAllTestsJSON();
        return 0;
    }

    ScreeningPipeline pipeline(25.0, 7.0, 3.0);

    // Headless / JSON mode for Dashboard integration
    if (argc >= 3 && std::string(argv[1]) == "--json") {
        std::string csvPath = argv[2];
        if (argc >= 4) pipeline.setLimits(std::stod(argv[3]), 7.0, 3.0);
        if (argc >= 5) pipeline.setLimits(std::stod(argv[3]), std::stod(argv[4]), 3.0);
        if (argc >= 6) pipeline.setLimits(std::stod(argv[3]), std::stod(argv[4]), std::stod(argv[5]));

        std::string err;
        if (!pipeline.loadFromCSV(csvPath, err)) {
            std::cerr << "{\"error\": \"" << err << "\"}\n";
            return 1;
        }

        std::string jsonOutput = pipeline.exportJSON();
        if (argc >= 7 && std::string(argv[6]) != "-") {
            std::ofstream out(argv[6]);
            out << jsonOutput;
            std::cout << "Successfully exported JSON to " << argv[6] << "\n";
        } else {
            std::cout << jsonOutput;
        }
        return 0;
    }

    // Interactive CLI mode
    displayBanner();

    int choice = -1;
    while (choice != 0) {
        std::cout << "\n--------------- MENU ---------------\n";
        std::cout << "1. Load Dataset from CSV\n";
        std::cout << "2. Display Lot Information (Linked List)\n";
        std::cout << "3. Search Component (Hash Table vs BST benchmark)\n";
        std::cout << "4. Detailed Component Analysis\n";
        std::cout << "5. Show Top Risk Components (Max Heap Priority Queue)\n";
        std::cout << "6. Display Lot Statistics (Merge Sort / Median / MAD)\n";
        std::cout << "7. Display Decision Summary Report\n";
        std::cout << "8. Data Structures & Viva Diagnostic Inspector\n";
        std::cout << "9. Export Screening Results to JSON file\n";
        std::cout << "10. Run Section 34 Verification Unit Tests\n";
        std::cout << "0. Exit\n";
        std::cout << "------------------------------------\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string discard;
            std::cin >> discard;
            continue;
        }

        if (choice == 1) {
            std::cout << "\nEnter CSV file path: ";
            std::string path;
            std::cin >> path;
            std::string err;
            auto start = std::chrono::high_resolution_clock::now();
            if (pipeline.loadFromCSV(path, err)) {
                auto end = std::chrono::high_resolution_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                std::cout << "[+] Successfully loaded and screened " << pipeline.getTotalComponents() 
                          << " components across " << pipeline.getLotStatistics().size() 
                          << " manufacturing lots in " << ms << " ms.\n";
                std::cout << "    PASS: " << pipeline.getPassCount() 
                          << " | MONITOR: " << pipeline.getMonitorCount()
                          << " | SUSPECT: " << pipeline.getSuspectCount()
                          << " | REJECT: " << pipeline.getRejectCount() << "\n";
            } else {
                std::cout << "[-] Failed to load CSV: " << err << "\n";
            }
        } 
        else if (choice == 2) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            std::cout << "\nAvailable manufacturing lots in dynamic linked list:\n";
            LotNode* node = pipeline.getLotDirectory().getHead();
            while (node) {
                std::cout << "  - Lot: " << node->lotID << " (" << node->componentList.size() << " components)\n";
                node = node->next;
            }
        } 
        else if (choice == 3) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            std::cout << "Enter Component ID to search: ";
            std::string id;
            std::cin >> id;

            // Search via Hash Table (O(1))
            auto t0 = std::chrono::high_resolution_clock::now();
            Component* fromHash = pipeline.searchByHash(id);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto hashTime = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

            // Search via BST (O(log n))
            auto t2 = std::chrono::high_resolution_clock::now();
            Component* fromBST = pipeline.searchByBST(id);
            auto t3 = std::chrono::high_resolution_clock::now();
            auto bstTime = std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count();

            if (fromHash && fromBST) {
                std::cout << "[+] Found component " << id << " in Lot " << fromHash->lotID << "\n";
                std::cout << "    Decision: " << fromHash->decision << " | Risk: " << fromHash->riskScore << "\n";
                std::cout << "    [Hash Table Search (O(1))] : " << hashTime << " ns\n";
                std::cout << "    [BST Search (O(log n))]    : " << bstTime << " ns\n";
                std::cout << "    [Verification] Both structures returned identical memory address: " << (fromHash == fromBST ? "YES" : "NO") << "\n";
            } else {
                std::cout << "[-] Component " << id << " not found in dataset.\n";
            }
        } 
        else if (choice == 4) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            std::cout << "Enter Component ID for analysis card: ";
            std::string id;
            std::cin >> id;
            Component* c = pipeline.searchByHash(id);
            printComponentCard(c);
        } 
        else if (choice == 5) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            int k = 10;
            std::cout << "How many top-risk components to extract from Max Heap (default 10): ";
            std::cin >> k;
            if (k <= 0) k = 10;

            std::vector<Component*> topList = pipeline.getTopRiskComponents(k);
            std::cout << "\n=========================================================================\n";
            std::cout << "           TOP RISK COMPONENTS (MAX HEAP PRIORITY QUEUE)\n";
            std::cout << "=========================================================================\n";
            std::cout << std::left << std::setw(6) << "Rank"
                      << std::setw(12) << "Component"
                      << std::setw(10) << "Lot"
                      << std::setw(12) << "Risk Score"
                      << std::setw(12) << "Decision"
                      << "Primary Reason\n";
            std::cout << "-------------------------------------------------------------------------\n";
            for (size_t i = 0; i < topList.size(); ++i) {
                Component* c = topList[i];
                std::cout << std::left << std::setw(6) << (i + 1)
                          << std::setw(12) << c->componentID
                          << std::setw(10) << c->lotID
                          << std::setw(12) << std::fixed << std::setprecision(2) << c->riskScore
                          << std::setw(12) << c->decision
                          << c->reason.substr(0, 36) << "...\n";
            }
            std::cout << "=========================================================================\n";
        } 
        else if (choice == 6) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            std::cout << "\n=======================================================================================\n";
            std::cout << "                  LOT-LEVEL STATISTICAL AUDIT (MEDIAN & MAD)\n";
            std::cout << "=======================================================================================\n";
            std::cout << std::left << std::setw(10) << "Lot"
                      << std::setw(8) << "Count"
                      << std::setw(16) << "Med/MAD L24"
                      << std::setw(16) << "Med/MAD I24"
                      << std::setw(16) << "Med/MAD L-Drift"
                      << "Med/MAD I-Drift\n";
            std::cout << "---------------------------------------------------------------------------------------\n";
            for (auto const& [lotID, st] : pipeline.getLotStatistics()) {
                std::ostringstream l24, i24, lDrift, iDrift;
                l24 << std::fixed << std::setprecision(1) << st.medianLeakage24h << "/" << st.madLeakage24h;
                i24 << std::fixed << std::setprecision(1) << st.medianIddq24h << "/" << st.madIddq24h;
                lDrift << std::fixed << std::setprecision(2) << st.medianLeakageDrift << "/" << st.madLeakageDrift;
                iDrift << std::fixed << std::setprecision(2) << st.medianIddqDrift << "/" << st.madIddqDrift;

                std::cout << std::left << std::setw(10) << lotID
                          << std::setw(8) << st.count
                          << std::setw(16) << l24.str()
                          << std::setw(16) << i24.str()
                          << std::setw(16) << lDrift.str()
                          << iDrift.str() << "\n";
            }
            std::cout << "=======================================================================================\n";
        } 
        else if (choice == 7) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            std::cout << "\n========================================\n";
            std::cout << "        SCREENING DECISION REPORT\n";
            std::cout << "========================================\n";
            int total = pipeline.getTotalComponents();
            std::cout << "Total Components : " << total << "\n";
            std::cout << "PASS             : " << pipeline.getPassCount() << " (" << std::fixed << std::setprecision(1) << (pipeline.getPassCount() * 100.0 / total) << "%)\n";
            std::cout << "MONITOR          : " << pipeline.getMonitorCount() << " (" << (pipeline.getMonitorCount() * 100.0 / total) << "%)\n";
            std::cout << "SUSPECT          : " << pipeline.getSuspectCount() << " (" << (pipeline.getSuspectCount() * 100.0 / total) << "%)\n";
            std::cout << "REJECT           : " << pipeline.getRejectCount() << " (" << (pipeline.getRejectCount() * 100.0 / total) << "%)\n";
            std::cout << "----------------------------------------\n";
            std::cout << "Engineering Limits:\n";
            std::cout << "  Leakage Limit  : " << pipeline.getLeakageLimit() << " uA\n";
            std::cout << "  IDDQ Limit     : " << pipeline.getIddqLimit() << " uA\n";
            std::cout << "  Z-Threshold    : " << pipeline.getZThreshold() << " MAD\n";
            std::cout << "========================================\n";
        } 
        else if (choice == 8) {
            std::cout << "\n=======================================================\n";
            std::cout << "      DATA STRUCTURES INSPECTION & VIVA EXPLANATION\n";
            std::cout << "=======================================================\n";
            std::cout << "1. ARRAY STORAGE: leakage[3] and iddq[3] inside struct\n";
            std::cout << "   - O(1) index access: 0->0h, 1->24h, 2->168h forecast.\n";
            std::cout << "\n2. DYNAMIC LINKED LIST (Lot-wise):\n";
            std::cout << "   - Active lots in directory: " << pipeline.getLotDirectory().size() << "\n";
            std::cout << "   - O(1) dynamic node insertion at lot head/tail.\n";
            std::cout << "\n3. HASH TABLE (Separate Chaining):\n";
            std::cout << "   - Capacity: " << pipeline.getHashTable().getTableCapacity() << " buckets (prime)\n";
            std::cout << "   - Occupied buckets: " << pipeline.getHashTable().getOccupiedBucketCount() << "\n";
            std::cout << "   - Collisions: " << pipeline.getHashTable().getCollisionCount() << "\n";
            std::cout << "   - Max chain length: " << pipeline.getHashTable().getMaxChainLength() << "\n";
            std::cout << "   - Load factor (alpha): " << pipeline.getHashTable().getLoadFactor() << "\n";
            std::cout << "\n4. BINARY SEARCH TREE (BST):\n";
            std::cout << "   - Nodes stored: " << pipeline.getBST().size() << "\n";
            std::cout << "   - Tree height: " << pipeline.getBST().getHeight() << "\n";
            std::cout << "   - Inorder traversal guarantees lexicographical ID order.\n";
            std::cout << "\n5. MAX HEAP PRIORITY QUEUE:\n";
            std::cout << "   - Array-based complete binary tree size: " << pipeline.getMaxHeap().size() << "\n";
            std::cout << "   - Peek max root risk: " << (pipeline.getMaxHeap().peekMax() ? pipeline.getMaxHeap().peekMax()->riskScore : 0.0) << "\n";
            std::cout << "   - O(1) max risk retrieval, O(log n) extractMax & insert.\n";
            std::cout << "=======================================================\n";
        } 
        else if (choice == 9) {
            if (pipeline.getTotalComponents() == 0) {
                std::cout << "[-] Please load a dataset first.\n";
                continue;
            }
            std::cout << "Enter output JSON path (e.g. results.json): ";
            std::string outPath;
            std::cin >> outPath;
            std::ofstream out(outPath);
            if (out.is_open()) {
                out << pipeline.exportJSON();
                std::cout << "[+] Results exported to " << outPath << "\n";
            } else {
                std::cout << "[-] Failed to write to " << outPath << "\n";
            }
        }
        else if (choice == 10) {
            Tests::runAllTests(true);
        }
    }

    std::cout << "\nExiting PRAKASH-DS. Goodbye!\n";
    return 0;
}
