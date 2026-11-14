# PRAKASH-DS (v1.0)
## Predictive Burn-in Semiconductor Screening & Risk Prioritization System
### Advanced Data Structures & Algorithms Course Project

---

## 1. Executive Summary & Engineering Context

During semiconductor manufacturing, integrated circuits (ICs) undergo high-temperature burn-in (e.g., $125^\circ\text{C}$ stress testing) to weed out infant mortality defects before products are shipped to high-reliability applications (automotive, aerospace, data centers).

Traditional screening relies solely on static limit testing at 168 hours. **PRAKASH-DS** is a high-performance Data Structures & Algorithms pipeline that evaluates early 24-hour telemetry measurements ($x_{0h}$, $x_{24h}$), isolates anomalous drift from natural lot process variations using non-parametric statistics (Median & MAD), projects end-of-test 168h degradation, and prioritizes latent defects using classical data structures.

---

## 2. Mapping of the Six Core Data Structures

Every data structure in **PRAKASH-DS** was chosen to solve a concrete engineering bottleneck in the screening pipeline:

| # | Data Structure | Implementation | Role & Functional Justification | Time Complexity | Space Complexity |
|---|---|---|---|---|---|
| **1** | **Fixed-Size Arrays** | `double leakage[3]`, `double iddq[3]` | Sequential storage for the three key timepoints: index `0` ($0h$), index `1` ($24h$), and index `2` ($168h$ forecast). Fixed contiguous memory eliminates allocation overhead. | Access: $O(1)$ | $O(1)$ per component |
| **2** | **Structures (`struct`)** | `struct Component`, `struct LotStatistics` | Encapsulates heterogeneous telemetry records: raw measurements, degradation drifts, statistical $Z$-scores, 168h linear projections, and categorical decisions. | Allocation: $O(1)$ | $O(1)$ per struct |
| **3** | **Dynamic Linked List** | `LotLinkedList`, `LotDirectory` | Dynamically aggregates components into manufacturing lots. Enables $O(1)$ insertion at lot tail without pre-specifying lot sizes or causing reallocations. | Append: $O(1)$<br>Traversal: $O(n)$ | $O(n)$ |
| **4** | **Separate Chaining Hash Table** | `ComponentHashTable` (Prime bucket size $1009$, DJB2 hash) | Fast random-access indexing. Enables instant $O(1)$ lookup of any component by ID (e.g., operator barcode scanning) with linked list collision resolution. | Search: Average $O(1)$, Worst $O(n)$ | $O(n + B)$ where $B$ is buckets |
| **5** | **Binary Search Tree (BST)** | `ComponentBST` | Hierarchical ordered storage keyed on alphanumeric Component ID. Provides $O(\log n)$ lookup and deterministic $O(n)$ in-order traversal to yield sorted serial exports without extra sorting. | Search: Avg $O(\log n)$, Worst $O(n)$<br>Inorder: $O(n)$ | $O(n)$ |
| **6** | **Max Heap Priority Queue** | `ComponentMaxHeap` (Complete binary tree in array) | Risk prioritization. The root always stores the component with the highest combined risk score. Quality engineers immediately inspect top-$K$ anomalous ICs via $O(1)$ peek and $O(\log n)$ extraction. | Build: $O(n)$ (Floyd)<br>Peek: $O(1)$<br>Extract: $O(\log n)$ | $O(n)$ |

---

## 3. Mathematical & Screening Formulations

### 3.1 Degradation Drift
Burn-in drift over the early 24-hour test window is computed as:
$$\Delta_{\text{leakage}} = x_{24h} - x_{0h}, \quad \Delta_{\text{iddq}} = y_{24h} - y_{0h}$$

### 3.2 Non-Parametric Lot Statistics (Merge Sort, Median & MAD)
Because semiconductor lots often contain extreme outlier components, conventional mean ($\mu$) and standard deviation ($\sigma$) suffer from masking and severe bias. PRAKASH-DS sorts measurements using **Merge Sort ($O(n \log n)$)** to compute robust metrics:
- **Median ($\tilde{x}$):** Middle value of the sorted distribution.
- **Median Absolute Deviation (MAD):**
  $$\text{MAD} = \text{Median}\left(\left|x_i - \tilde{x}\right|\right)$$
- **Robust Deviation Score ($Z$):**
  $$Z = \frac{|x_i - \tilde{x}|}{\text{MAD} + \epsilon}$$
  *(where $\epsilon = 10^{-6}$ protects against division by zero in tightly quantized lots).*

### 3.3 Linear 168h Extrapolation
Assuming early degradation rate is maintained over the burn-in cycle:
$$\text{Rate} = \frac{x_{24h} - x_{0h}}{24}$$
$$x_{168\text{ pred}} = x_{24h} + \text{Rate} \times (168 - 24) = 7 \cdot x_{24h} - 6 \cdot x_{0h}$$

### 3.4 Decision Hierarchy (Failsafe Engineering Rules)
1. **Deterministic Engineering Limit Check:**
   $$\text{If } \max(x_{0h}, x_{24h}) > \text{USL}_{\text{leakage}} \ (25.0\,\mu\text{A}) \text{ or } \max(y_{0h}, y_{24h}) > \text{USL}_{\text{iddq}} \ (7.0\,\mu\text{A}) \implies \mathbf{REJECT}$$
2. **Abnormal Indicator Tally (for non-rejected components):**
   - Indicator 1 (Position Outlier): $Z_{\text{pos}} = \max(Z_{\text{leak}}, Z_{\text{iddq}}) \ge 3.0$
   - Indicator 2 (Drift Outlier): $Z_{\text{drift}} = \max(Z_{\Delta\text{leak}}, Z_{\Delta\text{iddq}}) \ge 3.0$
   - Indicator 3 (168h Forecast Breach): $x_{168\text{ pred}} > \text{USL}_{\text{leakage}}$ or $y_{168\text{ pred}} > \text{USL}_{\text{iddq}}$
3. **Screening Classification:**
   - $0$ indicators $\implies \mathbf{PASS}$
   - $1$ indicator $\implies \mathbf{MONITOR}$
   - $2+$ indicators $\implies \mathbf{SUSPECT}$

### 3.5 Max Heap Combined Risk Prioritization
$$\text{Risk Score} = 0.35 \cdot Z_{\text{pos}} + 0.45 \cdot Z_{\text{drift}} + 0.20 \cdot \text{ForecastRisk} + (50.0 \text{ if REJECT})$$

---

## 4. Section 34 Verification Suite

The C++ core implements the automated unit tests specified in Section 34 of the project specifications:

```text
=======================================================================
          PRAKASH-DS: SECTION 34 SYSTEM VERIFICATION SUITE
=======================================================================
[PASS] Test 1: Normal Component              Expected: PASS     Actual: PASS
[PASS] Test 2: One Abnormal Indicator         Expected: MONITOR  Actual: MONITOR
[PASS] Test 3: Multiple Abnormal Indicators   Expected: SUSPECT  Actual: SUSPECT
[PASS] Test 4: Limit Violation               Expected: REJECT   Actual: REJECT
[PASS] Test 5: Hash Search                   Expected: Found    Actual: Component Found
[PASS] Test 6: Missing Component             Expected: Missing  Actual: Component Not Found
[PASS] Test 7: Max Heap Priority Queue       Expected: Max 1st  Actual: Highest score appears first
[PASS] Test 8: BST Inorder Traversal         Expected: Sorted   Actual: Sorted order
=======================================================================
SUMMARY: 8 / 8 tests passed (100.0%)
=======================================================================
```

---

## 5. Project Directory Structure

```
prakash-ds/
│
├── cpp/                           # C++17 Core Engine Source Files
│   ├── component.h / .cpp         # Struct Component & Array measurements
│   ├── linkedlist.h / .cpp        # LotLinkedList & LotDirectory
│   ├── hashtable.h / .cpp         # Separate Chaining Hash Table (DJB2)
│   ├── bst.h / .cpp               # Binary Search Tree & Inorder Traversal
│   ├── maxheap.h / .cpp           # Complete Binary Tree Max Heap
│   ├── statistics.h / .cpp        # Merge Sort, Median, MAD, Robust Z
│   ├── screening.h / .cpp         # Pipeline Orchestration & JSON export
│   ├── tests.h / .cpp             # Section 34 Automated Unit Tests
│   └── main.cpp                   # CLI Menu & Headless Flag Parser
│
├── server/
│   └── server.py                  # Lightweight HTTP & REST API Gateway
│
├── public/                        # Standalone Web Dashboard (100% Offline)
│   ├── index.html                 # 4-Tab Interface matching design inspiration
│   ├── styles.css                 # Semiconductor Telemetry & Glassmorphism Theme
│   └── app.js                     # High-DPI Canvas & SVG Graphics Engine
│
├── data/
│   ├── dataset.csv                # Master 6,000 components dataset (LOT-01 to LOT-08)
│   ├── dataset_healthy.csv        # 2,000 healthy components (H0001 - H2000)
│   ├── dataset_monitor.csv        # 2,000 monitor components (M0001 - M2000)
│   └── dataset_reject.csv         # 2,000 reject components (R0001 - R2000)
│
├── sample_data/                   # 1-Click Quick-Load Demonstration Lots
│   ├── lot_demo_a_1000.csv        # 1,000-component lot matching UI inspiration
│   ├── healthy_baseline_lot.csv   # 500-component nominal lot
│   ├── high_stress_drift_lot.csv  # 500-component accelerated stress lot
│   └── full_6000_components.csv  # Full 6,000 components dataset
│
├── build.bat                      # Windows batch build script for G++
├── start.bat                      # One-click dashboard launcher
├── prakash_ds.exe                 # Compiled native 64-bit C++ executable
└── README.md                      # Comprehensive documentation & viva guide
```

---

## 6. How to Run

### Option A: One-Click Web Dashboard (Recommended)
Double-click `start.bat` or run:
```bat
start.bat
```
This automatically compiles the C++ binary (if needed), starts the local Python REST server, and opens your browser at `http://localhost:8000`.

### Option B: Interactive C++ CLI Console
To run the terminal menu directly:
```bat
prakash_ds.exe
```
Menu options:
1. Load Dataset from CSV
2. Display Lot Information (Linked List)
3. Search Component (Hash Table vs BST benchmark)
4. Detailed Component Analysis
5. Show Top Risk Components (Max Heap Priority Queue)
6. Display Lot Statistics (Merge Sort / Median / MAD)
7. Display Decision Summary Report
8. Data Structures & Viva Diagnostic Inspector
9. Export Screening Results to JSON file
10. Run Section 34 Verification Unit Tests

### Option C: Run Section 34 Unit Tests Directly
```bat
prakash_ds.exe --test
```

### Option D: Recompile C++ Core
```bat
build.bat
```
*(Uses `g++ -O3 -std=c++17 cpp/*.cpp -o prakash_ds.exe`)*

---

## 7. Viva Defense & Examination Cheat Sheet

**Q1: Why did you use Median and MAD instead of Mean and Standard Deviation?**
> *Answer:* Standard deviation assumes a Gaussian distribution and squares errors, meaning a single extreme defect (e.g., an IC drawing $30\,\mu\text{A}$ instead of $9\,\mu\text{A}$) artificially inflates the variance, causing nearby latent defects to appear normal ("masking effect"). Median and MAD are non-parametric and have a $50\%$ breakdown point, ensuring reliable lot baselines even if half the lot is degraded.

**Q2: What is the load factor of your Hash Table and how are collisions resolved?**
> *Answer:* We use a prime bucket capacity of $1,009$ with separate chaining. When inserting $6,000$ components, the load factor is $\alpha \approx 5.9$, resulting in short linked lists of length $5\text{--}6$ that yield sub-microsecond retrieval ($< 0.1\,\mu\text{s}$), outperforming linear searches by over $200\times$.

**Q3: How does the Max Heap optimize risk prioritization over regular sorting?**
> *Answer:* Sorting $6,000$ components with Quick Sort or Merge Sort takes $O(n \log n)$ time. With a Max Heap, we construct the complete binary tree in $O(n)$ time using Floyd's bottom-up `heapifyDown` algorithm. Finding the highest-risk component is $O(1)$ (`peekMax()`), and retrieving the top $K$ risky parts takes $O(K \log n)$, which is significantly faster when $K \ll n$.

**Q4: Why maintain both a Hash Table and a BST for the same components?**
> *Answer:* A Hash Table provides average $O(1)$ lookup, but elements are stored in pseudorandom bucket order; extracting sorted keys requires an $O(n \log n)$ post-sort. A BST preserves key order naturally; an in-order traversal visits left-subtree, root, right-subtree to generate lexicographically sorted serial exports in $O(n)$ time with zero additional sorting passes.
