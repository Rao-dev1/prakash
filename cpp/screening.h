#ifndef SCREENING_H
#define SCREENING_H

#include "component.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "bst.h"
#include "maxheap.h"
#include <string>
#include <vector>
#include <map>

// Lot-level statistical summary for audit and reporting
struct LotStatistics {
    std::string lotID;
    int count;

    double medianLeakage0h;
    double madLeakage0h;
    double medianLeakage24h;
    double madLeakage24h;
    double medianLeakage168hPred;
    double madLeakage168hPred;

    double medianIddq0h;
    double madIddq0h;
    double medianIddq24h;
    double madIddq24h;
    double medianIddq168hPred;
    double madIddq168hPred;

    double medianLeakageDrift;
    double madLeakageDrift;
    double medianIddqDrift;
    double madIddqDrift;
};

// Complete PRAKASH-DS Screening Pipeline & Data Structure Orchestrator
class ScreeningPipeline {
private:
    // Core parameters (Configurable)
    double leakageLimit; // Deterministic engineering threshold (uA)
    double iddqLimit;    // Deterministic engineering threshold (uA)
    double zThreshold;   // Abnormality threshold (Standard 3.0 MAD)

    // Master list of allocated components
    std::vector<Component*> allComponents;

    // The Six Core Data Structures:
    // 1. Arrays: Stored inside Component (leakage[3], iddq[3])
    // 2. Structs: Component
    // 3. Dynamic Linked List: LotDirectory & LotLinkedList
    LotDirectory lotDirectory;
    // 4. Hash Table: ComponentHashTable (O(1) search)
    ComponentHashTable hashTable;
    // 5. Binary Search Tree: ComponentBST (O(log n) ordered search)
    ComponentBST bst;
    // 6. Max Heap: ComponentMaxHeap (Priority queue for risk)
    ComponentMaxHeap maxHeap;

    // Lot-level statistics
    std::map<std::string, LotStatistics> lotStatsMap;

    // Screening decision tallies
    int passCount;
    int monitorCount;
    int suspectCount;
    int rejectCount;

public:
    ScreeningPipeline(double lLimit = 25.0, double iLimit = 7.0, double zThresh = 3.0);
    ~ScreeningPipeline();

    void setLimits(double lLimit, double iLimit, double zThresh);
    double getLeakageLimit() const { return leakageLimit; }
    double getIddqLimit() const { return iddqLimit; }
    double getZThreshold() const { return zThreshold; }

    // CSV ingestion & parsing
    bool loadFromCSV(const std::string& filePath, std::string& errorMsg);

    // Executes the lot-based screening algorithms across all data structures
    void runScreening();

    // Clear all data
    void clear();

    // Data Structure Accessors
    const LotDirectory& getLotDirectory() const { return lotDirectory; }
    const ComponentHashTable& getHashTable() const { return hashTable; }
    const ComponentBST& getBST() const { return bst; }
    const ComponentMaxHeap& getMaxHeap() const { return maxHeap; }
    const std::vector<Component*>& getAllComponents() const { return allComponents; }
    const std::map<std::string, LotStatistics>& getLotStatistics() const { return lotStatsMap; }

    // Fast search using Hash Table: O(1)
    Component* searchByHash(const std::string& id) const {
        return hashTable.search(id);
    }

    // Ordered search using BST: O(log n)
    Component* searchByBST(const std::string& id) const {
        return bst.search(id);
    }

    // Top-risk components using Max Heap Priority Queue: O(k log n)
    std::vector<Component*> getTopRiskComponents(int k = 10) const {
        return maxHeap.getTopK(k);
    }

    // KPI Summary
    int getTotalComponents() const { return static_cast<int>(allComponents.size()); }
    int getPassCount() const { return passCount; }
    int getMonitorCount() const { return monitorCount; }
    int getSuspectCount() const { return suspectCount; }
    int getRejectCount() const { return rejectCount; }

    // JSON export method for local dashboard server
    std::string exportJSON() const;
};

#endif // SCREENING_H
