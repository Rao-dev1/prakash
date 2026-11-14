#include "tests.h"
#include "component.h"
#include "hashtable.h"
#include "bst.h"
#include "maxheap.h"
#include "screening.h"
#include "statistics.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

namespace Tests {

    struct TestCaseResult {
        int id;
        std::string name;
        std::string expected;
        std::string actual;
        bool passed;
        std::string notes;
    };

    static std::vector<TestCaseResult> executeTests() {
        std::vector<TestCaseResult> results;

        // --- Test 1: Normal Component ---
        {
            ScreeningPipeline pipeline(25.0, 7.0, 3.0);
            std::stringstream ss;
            ss << "component_id,lot_id,leakage_0h,leakage_24h,iddq_0h,iddq_24h\n";
            // Realistic baseline lot with slight natural measurement variance
            for (int i = 1; i <= 25; ++i) {
                double l0 = 8.30 + (i % 5) * 0.10; // 8.30, 8.40, 8.50, 8.60, 8.70 (median 8.50, MAD 0.10)
                double l24 = l0 + 0.10;            // drift 0.10
                double i0 = 2.30 + (i % 5) * 0.05; // 2.30 to 2.50
                double i24 = i0 + 0.05;            // drift 0.05
                ss << "N" << std::setw(3) << std::setfill('0') << i << ",LOT-TEST,"
                   << std::fixed << std::setprecision(2) << l0 << "," << l24 << "," << i0 << "," << i24 << "\n";
            }
            // Target normal component right at median with nominal drift
            ss << "N099,LOT-TEST,8.50,8.60,2.40,2.45\n";

            std::string tempPath = "temp_test_lot1.csv";
            {
                std::ofstream f(tempPath);
                f << ss.str();
            }

            std::string err;
            pipeline.loadFromCSV(tempPath, err);
            remove(tempPath.c_str());

            Component* c = pipeline.searchByHash("N099");
            std::string actual = c ? c->decision : "NOT_FOUND";
            bool ok = (actual == "PASS");
            results.push_back({1, "Test 1 --- Normal Component", "PASS", actual, ok, "Zero abnormal indicators within lot population"});
        }

        // --- Test 2: One Abnormal Indicator ---
        {
            ScreeningPipeline pipeline(25.0, 7.0, 3.0);
            std::stringstream ss;
            ss << "component_id,lot_id,leakage_0h,leakage_24h,iddq_0h,iddq_24h\n";
            for (int i = 1; i <= 25; ++i) {
                double l0 = 8.30 + (i % 5) * 0.10;
                double l24 = l0 + 0.10;
                double i0 = 2.30 + (i % 5) * 0.05;
                double i24 = i0 + 0.05;
                ss << "M_BASE" << i << ",LOT-TEST,"
                   << std::fixed << std::setprecision(2) << l0 << "," << l24 << "," << i0 << "," << i24 << "\n";
            }
            // Normal 24h position (8.60 uA equals lot median 8.60 uA, Position Z = 0.0 < 3.0)
            // Elevated drift (8.10 -> 8.60 = 0.50 uA drift vs lot median drift 0.10 uA -> Drift Z >= 3.0)
            // 168h forecast: 7 * 8.60 - 6 * 8.10 = 11.60 uA (< 25.0 uA limit, safe)
            // Exactly 1 abnormal indicator -> MONITOR
            ss << "M_TARGET,LOT-TEST,8.10,8.60,2.40,2.45\n";

            std::string tempPath = "temp_test_lot2.csv";
            {
                std::ofstream f(tempPath);
                f << ss.str();
            }

            std::string err;
            pipeline.loadFromCSV(tempPath, err);
            remove(tempPath.c_str());

            Component* c = pipeline.searchByHash("M_TARGET");
            std::string actual = c ? c->decision : "NOT_FOUND";
            bool ok = (actual == "MONITOR");
            results.push_back({2, "Test 2 --- One Abnormal Indicator", "MONITOR", actual, ok, "Exactly 1 abnormal indicator (elevated drift Z >= 3.0)"});
        }

        // --- Test 3: Multiple Abnormal Indicators ---
        {
            ScreeningPipeline pipeline(25.0, 7.0, 3.0);
            std::stringstream ss;
            ss << "component_id,lot_id,leakage_0h,leakage_24h,iddq_0h,iddq_24h\n";
            for (int i = 1; i <= 25; ++i) {
                double l0 = 8.30 + (i % 5) * 0.10;
                double l24 = l0 + 0.10;
                double i0 = 2.30 + (i % 5) * 0.05;
                double i24 = i0 + 0.05;
                ss << "S_BASE" << i << ",LOT-TEST,"
                   << std::fixed << std::setprecision(2) << l0 << "," << l24 << "," << i0 << "," << i24 << "\n";
            }
            // High initial position + high drift + forecast breach (7 * 16 - 6 * 12.5 = 37 uA > 25 limit)
            ss << "S_TARGET,LOT-TEST,12.50,16.00,3.50,4.80\n";

            std::string tempPath = "temp_test_lot3.csv";
            {
                std::ofstream f(tempPath);
                f << ss.str();
            }

            std::string err;
            pipeline.loadFromCSV(tempPath, err);
            remove(tempPath.c_str());

            Component* c = pipeline.searchByHash("S_TARGET");
            std::string actual = c ? c->decision : "NOT_FOUND";
            bool ok = (actual == "SUSPECT");
            results.push_back({3, "Test 3 --- Multiple Abnormal Indicators", "SUSPECT", actual, ok, "Position Z >= 3.0, Drift Z >= 3.0, Forecast breach"});
        }

        // --- Test 4: Engineering Limit Violation ---
        {
            ScreeningPipeline pipeline(25.0, 7.0, 3.0);
            std::stringstream ss;
            ss << "component_id,lot_id,leakage_0h,leakage_24h,iddq_0h,iddq_24h\n";
            for (int i = 1; i <= 25; ++i) {
                double l0 = 8.30 + (i % 5) * 0.10;
                double l24 = l0 + 0.10;
                double i0 = 2.30 + (i % 5) * 0.05;
                double i24 = i0 + 0.05;
                ss << "R_BASE" << i << ",LOT-TEST,"
                   << std::fixed << std::setprecision(2) << l0 << "," << l24 << "," << i0 << "," << i24 << "\n";
            }
            // Exceeds deterministic 25.0 uA limit
            ss << "R_TARGET,LOT-TEST,8.50,28.50,2.40,3.00\n";

            std::string tempPath = "temp_test_lot4.csv";
            {
                std::ofstream f(tempPath);
                f << ss.str();
            }

            std::string err;
            pipeline.loadFromCSV(tempPath, err);
            remove(tempPath.c_str());

            Component* c = pipeline.searchByHash("R_TARGET");
            std::string actual = c ? c->decision : "NOT_FOUND";
            bool ok = (actual == "REJECT");
            results.push_back({4, "Test 4 --- Engineering Limit Violation", "REJECT", actual, ok, "Exceeds deterministic USL limit (28.50 uA > 25.0 uA)"});
        }

        // --- Test 5: Hash Search Found ---
        {
            ComponentHashTable ht;
            Component dummy("COMP-FOUND-001", "LOT-01", 8.0, 8.2, 2.0, 2.1);
            ht.insert("COMP-FOUND-001", &dummy);

            Component* found = ht.search("COMP-FOUND-001");
            bool ok = (found != nullptr && found->componentID == "COMP-FOUND-001");
            std::string actual = ok ? "Component Found" : "Component Not Found";
            results.push_back({5, "Test 5 --- Hash Search", "Component Found", actual, ok, "Separate chaining O(1) key lookup"});
        }

        // --- Test 6: Missing Component ---
        {
            ComponentHashTable ht;
            Component dummy("COMP-FOUND-001", "LOT-01", 8.0, 8.2, 2.0, 2.1);
            ht.insert("COMP-FOUND-001", &dummy);

            Component* missing = ht.search("COMP-NONEXISTENT-999");
            bool ok = (missing == nullptr);
            std::string actual = ok ? "Component Not Found" : "Component Found (Unexpected)";
            results.push_back({6, "Test 6 --- Missing Component", "Component Not Found", actual, ok, "Correctly returns nullptr for absent key"});
        }

        // --- Test 7: Max Heap ---
        {
            ComponentMaxHeap heap;
            Component c1("C1", "LOT-01", 10.0, 10.0, 2.0, 2.0); c1.riskScore = 15.2;
            Component c2("C2", "LOT-01", 10.0, 10.0, 2.0, 2.0); c2.riskScore = 89.7;
            Component c3("C3", "LOT-01", 10.0, 10.0, 2.0, 2.0); c3.riskScore = 42.1;
            Component c4("C4", "LOT-01", 10.0, 10.0, 2.0, 2.0); c4.riskScore = 95.4;
            Component c5("C5", "LOT-01", 10.0, 10.0, 2.0, 2.0); c5.riskScore = 5.0;

            heap.insert(&c1);
            heap.insert(&c2);
            heap.insert(&c3);
            heap.insert(&c4);
            heap.insert(&c5);

            Component* max1 = heap.peekMax();
            bool peekOk = (max1 && max1->riskScore == 95.4);

            bool monotonicDesc = true;
            double prevRisk = 1e9;
            while (!heap.isEmpty()) {
                Component* top = heap.extractMax();
                if (top->riskScore > prevRisk) {
                    monotonicDesc = false;
                    break;
                }
                prevRisk = top->riskScore;
            }

            bool ok = peekOk && monotonicDesc;
            std::string actual = ok ? "Highest score appears first" : "Heap ordering violated";
            results.push_back({7, "Test 7 --- Max Heap Priority Queue", "Highest score appears first", actual, ok, "peekMax: 95.4; extractMax sequence: 95.4 >= 89.7 >= 42.1 >= 15.2 >= 5.0"});
        }

        // --- Test 8: BST Inorder Traversal ---
        {
            ComponentBST bst;
            Component cE("LOT-E", "L", 10.0, 10.0, 2.0, 2.0);
            Component cB("LOT-B", "L", 10.0, 10.0, 2.0, 2.0);
            Component cA("LOT-A", "L", 10.0, 10.0, 2.0, 2.0);
            Component cD("LOT-D", "L", 10.0, 10.0, 2.0, 2.0);
            Component cC("LOT-C", "L", 10.0, 10.0, 2.0, 2.0);

            bst.insert(&cE);
            bst.insert(&cB);
            bst.insert(&cA);
            bst.insert(&cD);
            bst.insert(&cC);

            std::vector<Component*> inorder = bst.getInorder();
            bool isSorted = true;
            for (size_t i = 1; i < inorder.size(); ++i) {
                if (inorder[i-1]->componentID >= inorder[i]->componentID) {
                    isSorted = false;
                    break;
                }
            }

            bool ok = isSorted && (inorder.size() == 5) && (inorder[0]->componentID == "LOT-A") && (inorder[4]->componentID == "LOT-E");
            std::string actual = ok ? "Sorted order" : "Unsorted sequence";
            results.push_back({8, "Test 8 --- BST Inorder Traversal", "Sorted order", actual, ok, "Inorder yields: LOT-A, LOT-B, LOT-C, LOT-D, LOT-E"});
        }

        return results;
    }

    bool runAllTests(bool verbose) {
        auto results = executeTests();
        int passed = 0;

        if (verbose) {
            std::cout << "\n=======================================================================\n";
            std::cout << "          PRAKASH-DS: SECTION 34 SYSTEM VERIFICATION SUITE\n";
            std::cout << "=======================================================================\n";
        }

        for (const auto& r : results) {
            if (r.passed) passed++;
            if (verbose) {
                std::cout << "[" << (r.passed ? "PASS" : "FAIL") << "] Test " << r.id << ": " << r.name << "\n";
                std::cout << "       Expected : " << r.expected << "\n";
                std::cout << "       Actual   : " << r.actual << "\n";
                std::cout << "       Details  : " << r.notes << "\n";
                std::cout << "-----------------------------------------------------------------------\n";
            }
        }

        if (verbose) {
            std::cout << "SUMMARY: " << passed << " / " << results.size() << " tests passed ("
                      << std::fixed << std::setprecision(1) << (passed * 100.0 / results.size()) << "%)\n";
            std::cout << "=======================================================================\n\n";
        }

        return (passed == static_cast<int>(results.size()));
    }

    std::string runAllTestsJSON() {
        auto results = executeTests();
        int passed = 0;
        for (const auto& r : results) if (r.passed) passed++;

        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"totalTests\": " << results.size() << ",\n";
        ss << "  \"passedTests\": " << passed << ",\n";
        ss << "  \"successRate\": " << std::fixed << std::setprecision(1) << (passed * 100.0 / results.size()) << ",\n";
        ss << "  \"allPassed\": " << (passed == static_cast<int>(results.size()) ? "true" : "false") << ",\n";
        ss << "  \"results\": [\n";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) ss << ",\n";
            ss << "    {\n";
            ss << "      \"id\": " << results[i].id << ",\n";
            ss << "      \"name\": \"" << results[i].name << "\",\n";
            ss << "      \"expected\": \"" << results[i].expected << "\",\n";
            ss << "      \"actual\": \"" << results[i].actual << "\",\n";
            ss << "      \"passed\": " << (results[i].passed ? "true" : "false") << ",\n";
            ss << "      \"notes\": \"" << results[i].notes << "\"\n";
            ss << "    }";
        }
        ss << "\n  ]\n";
        ss << "}\n";
        return ss.str();
    }
}
