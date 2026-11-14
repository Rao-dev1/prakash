# PRAKASH-DS

## Data-Structure-Based Burn-In Screening and Risk Prioritization System

**Project Type:** Data Structures & Algorithms Course Project\
**Language:** C++\
**Domain:** Semiconductor / Electronic Component Reliability Screening\
**Base System:** PRAKASH 1.2\
**Project Version:** PRAKASH-DS v1.0

------------------------------------------------------------------------

# 1. Project Overview

**PRAKASH-DS** is a simplified, data-structure-oriented version of the
PRAKASH 1.2 burn-in screening system.

The purpose of the project is to analyze burn-in measurements of
electronic components and identify components that may be abnormal,
degrading unusually fast, or likely to cross an engineering limit by the
end of the burn-in period.

The original PRAKASH 1.2 system contains several advanced statistical
and machine-learning methods. For a Data Structures course project,
those methods are intentionally simplified.

The **engineering purpose remains similar**, but the implementation is
redesigned around core Data Structures and Algorithms.

The system works with two electrical parameters:

-   **Leakage Current (`leakage_ua`)**
-   **IDDQ Current (`iddq_ua`)**

Measurements are collected at:

-   **0 hours**
-   **24 hours**
-   **168 hours**

For every component, PRAKASH-DS performs:

1.  Data storage and organization
2.  Lot-based comparison
3.  Abnormal-position detection
4.  Degradation/drift detection
5.  168-hour forecast
6.  Engineering-limit checking
7.  Risk classification
8.  Risk prioritization

The final decision is:

> **PASS / MONITOR / SUSPECT / REJECT**

------------------------------------------------------------------------

# 2. Relationship With PRAKASH 1.2

PRAKASH-DS is not a completely different project.

It preserves the central idea of PRAKASH 1.2:

> **Use early burn-in measurements to identify components that behave
> differently from their lot, degrade unusually, or are heading toward
> an unacceptable condition.**

The major difference is the implementation complexity.

  -----------------------------------------------------------------------
  PRAKASH 1.2                         PRAKASH-DS
  ----------------------------------- -----------------------------------
  Advanced statistical pipeline       Simplified algorithmic pipeline

  Four timepoints                     Three timepoints

  MCD Mahalanobis analysis            Median/MAD deviation

  Isolation Forest                    Removed

  Conditional Huber regression        Simplified drift comparison

  Pooled power-law forecast           Linear extrapolation

  Conformal prediction                Removed

  Multiple validation/ablation layers Basic validation

  Python/ML-oriented implementation   C++/DS-oriented implementation

  Data structures mostly supporting   Data structures are a core part of
  the pipeline                        the system
  -----------------------------------------------------------------------

The **goal and decision philosophy** remain aligned.

------------------------------------------------------------------------

# 3. Problem Statement

During burn-in testing, electronic components are exposed to controlled
operating conditions and measured at different points in time.

A component may appear normal initially but later show:

-   unusually high leakage,
-   unusually high IDDQ,
-   rapid degradation,
-   abnormal behavior compared with other components in the same lot,
-   or a predicted value that approaches/exceeds an engineering limit.

Checking only individual measurements is therefore insufficient.

PRAKASH-DS provides an algorithmic system that:

> **Stores, searches, compares, analyzes, predicts, and prioritizes
> burn-in measurements using core Data Structures and Algorithms.**

------------------------------------------------------------------------

# 4. Objectives

The main objectives are:

1.  Store component burn-in measurements efficiently.
2.  Organize components according to their lots.
3.  Quickly search for a component using its ID.
4.  Compare a component against its lot population.
5.  Detect unusual measurement position.
6.  Detect unusually high degradation.
7.  Estimate the component's future value at 168 hours.
8.  Apply engineering limits deterministically.
9.  Assign a screening decision.
10. Prioritize the highest-risk components for inspection.
11. Demonstrate practical use of core Data Structures and Algorithms.

------------------------------------------------------------------------

# 5. Core Data Structures Used

PRAKASH-DS deliberately uses **six core Data Structures**.

## 5.1 Arrays

Arrays store the time-series measurements of each component.

Example:

``` text
Leakage:
[10.2, 13.8, 42.7]

IDDQ:
[3.1, 3.6, 7.4]
```

The indexes represent:

``` text
Index 0 → 0 hours
Index 1 → 24 hours
Index 2 → 168 hours
```

### Why arrays?

The number of measurement points is fixed.

Arrays provide:

-   direct access,
-   simple storage,
-   O(1) access by index,
-   easy calculation of differences and forecasts.

------------------------------------------------------------------------

## 5.2 Structures / Records

A C++ `struct` represents one complete component.

Conceptually:

``` cpp
struct Component
{
    string componentID;
    string lotID;

    double leakage[3];
    double iddq[3];

    double positionScore;
    double driftScore;
    double forecast;

    string decision;
};
```

The structure groups all information belonging to one component.

### Why structures?

Without a structure, measurements and component information would be
scattered across separate variables.

The structure creates a single logical record.

------------------------------------------------------------------------

## 5.3 Linked List

Each lot can maintain a linked list of its components.

Example:

``` text
HEAD
 |
 v
C001 -> C002 -> C003 -> C004 -> NULL
```

A node contains a component record and a pointer to the next node.

### Why linked list?

The number of components in a lot may change.

A linked list allows components to be:

-   added,
-   removed,
-   traversed,
-   organized dynamically.

This demonstrates dynamic memory and linked-list operations.

------------------------------------------------------------------------

## 5.4 Hash Table

A hash table maps:

``` text
Component ID → Component
```

Example:

``` text
"C001" → Component C001
"C002" → Component C002
"C003" → Component C003
```

### Why hash table?

A user should be able to search for a component quickly.

Average search complexity:

``` text
O(1)
```

A simple implementation can use:

-   hash function,
-   array of entries,
-   collision handling using linear probing or separate chaining.

------------------------------------------------------------------------

## 5.5 Binary Search Tree

A Binary Search Tree can organize components according to a key such as:

-   component ID, or
-   risk score.

Example:

``` text
             C050
            /    \
         C020    C080
         /  \      \
      C010 C030   C090
```

### Why BST?

It demonstrates:

-   ordered storage,
-   searching,
-   insertion,
-   traversal.

Average search complexity:

``` text
O(log n)
```

Worst case:

``` text
O(n)
```

The BST can also be traversed in sorted order.

------------------------------------------------------------------------

## 5.6 Max Heap

The Max Heap is used to prioritize risky components.

Example:

``` text
          9.5
        /     \
      8.7      8.2
     /  \     /
   7.9  7.2  7.5
```

The largest risk score stays at the root.

This allows PRAKASH-DS to answer:

> "Which components should the engineer inspect first?"

### Complexity

Insertion:

``` text
O(log n)
```

Highest-risk component:

``` text
O(1)
```

Removing the highest-risk component:

``` text
O(log n)
```

------------------------------------------------------------------------

# 6. Why These Data Structures Are Not Artificially Added

The data structures are connected directly to actual operations in the
system.

  Data Structure   PRAKASH-DS Purpose
  ---------------- --------------------------------------------
  Array            Store measurements at different timepoints
  Structure        Represent a complete component
  Linked List      Dynamically maintain components in a lot
  Hash Table       Fast component-ID lookup
  BST              Ordered component searching/traversal
  Max Heap         Risk prioritization

Therefore, the project does not merely contain data structures for
demonstration.

Each structure solves a specific problem inside the application.

------------------------------------------------------------------------

# 7. Data Model

Each component has:

``` text
Component ID
Lot ID

Leakage Current:
    0h
    24h
    168h

IDDQ:
    0h
    24h
    168h

Position Score
Drift Score
Forecast

Decision
```

Example:

``` text
Component ID : C104
Lot ID       : LOT-A

Leakage:
0h   = 10.2 uA
24h  = 13.8 uA
168h = 42.7 uA

IDDQ:
0h   = 3.1 uA
24h  = 3.6 uA
168h = 7.4 uA
```

------------------------------------------------------------------------

# 8. Dataset Organization

A simple dataset can contain:

``` text
5 Lots
×
100 Components per Lot
=
500 Components
```

Each component has:

``` text
2 parameters
×
3 timepoints
=
6 measurements
```

Therefore:

``` text
500 × 6 = 3000 measurement values
```

The dataset can contain both:

-   normal components,
-   intentionally abnormal/high-drift components.

This allows the system to demonstrate screening behavior.

------------------------------------------------------------------------

# 9. Timepoints

PRAKASH-DS uses three timepoints:

``` text
T0   = 0 hours
T24  = 24 hours
T168 = 168 hours
```

The original PRAKASH 1.2 specification used four timepoints.

For the Data Structures project, the 96-hour point is removed to reduce
implementation complexity while preserving the main early-detection and
endpoint-forecasting behavior.

------------------------------------------------------------------------

# 10. Lot-Based Analysis

A component should not always be judged only against a fixed global
average.

Components belonging to the same manufacturing lot are compared against
their lot population.

For example:

``` text
LOT-A
 ├── C001
 ├── C002
 ├── C003
 ├── ...
 └── C100
```

The system calculates lot-level statistics for:

-   Leakage,
-   IDDQ,
-   Drift.

This preserves the population-relative philosophy of PRAKASH.

------------------------------------------------------------------------

# 11. Position Analysis

Position analysis answers:

> **"Is this component unusually high or low compared with other
> components in its lot?"**

For each parameter, calculate:

### Median

The median represents the typical value of the lot.

### MAD

MAD means:

> Median Absolute Deviation

It measures how spread out the lot measurements are around the median.

A simplified robust deviation score is:

``` text
Z = |x - median| / (MAD + ε)
```

where:

``` text
x      = component measurement
ε      = small value preventing division by zero
```

For each component:

``` text
Leakage Z-score
IDDQ Z-score
```

are calculated.

The position score can then be:

``` text
Position Score = max(Leakage Z, IDDQ Z)
```

A higher score means the component is more unusual relative to its lot.

------------------------------------------------------------------------

# 12. Drift Analysis

Position alone is not enough.

A component may start at a normal value but increase rapidly.

Therefore, PRAKASH-DS calculates drift.

For a parameter:

``` text
Drift = Value at 24h - Value at 0h
```

Example:

``` text
0h  = 10
24h = 15

Drift = 15 - 10
      = 5
```

The component's drift is then compared with the drift of other
components in the same lot.

A simplified drift score is:

``` text
Drift Score =
|Component Drift - Lot Median Drift|
/
(Lot Drift MAD + ε)
```

This answers:

> **"Is this component degrading faster than its peers?"**

------------------------------------------------------------------------

# 13. Forecasting

PRAKASH 1.2 uses a more complex forecast.

For PRAKASH-DS, the forecast is intentionally simplified.

The system assumes a linear continuation of the early trend.

For a parameter:

``` text
Rate = (x24 - x0) / 24
```

From 24 hours to 168 hours there are:

``` text
168 - 24 = 144 hours
```

Therefore:

``` text
x168_predicted
=
x24 + Rate × 144
```

This can also be written as:

``` text
x168_predicted = 7 × x24 - 6 × x0
```

Example:

``` text
x0  = 10
x24 = 15

Forecast
= 7(15) - 6(10)
= 105 - 60
= 45
```

The forecast asks:

> **"If the current trend continues, where could this component reach by
> 168 hours?"**

------------------------------------------------------------------------

# 14. Engineering Limits

Engineering limits have higher authority than statistical scores.

For example:

``` text
Leakage Limit = predefined engineering limit
IDDQ Limit    = predefined engineering limit
```

The exact limits should be supplied by the project/user dataset.

**The project must not invent aerospace or semiconductor qualification
limits and present them as real industry specifications.**

If a measured value violates an explicit engineering limit:

``` text
Decision = REJECT
```

This is deterministic.

------------------------------------------------------------------------

# 15. Decision Logic

PRAKASH-DS retains the important PRAKASH principle:

> **Statistical screening can identify risk, but it does not
> independently issue a REJECT.**

The simplified decision process is:

``` text
                Component
                    |
                    v
          Engineering Limit Check
              /           \
           FAIL            PASS
            |                |
         REJECT        Calculate Scores
                             |
                    +--------+--------+
                    |        |        |
                 Position   Drift   Forecast
                    |        |        |
                    +--------+--------+
                             |
                      Count Abnormal
                         Indicators
                             |
                +------------+------------+
                |            |            |
              Normal       1 abnormal   2+ abnormal
                |            |            |
               PASS       MONITOR       SUSPECT
```

### Decision categories

#### PASS

The component:

-   is within engineering limits,
-   has normal position,
-   has normal drift,
-   and has no significant forecast concern.

#### MONITOR

The component has one abnormal indicator but does not require immediate
rejection.

#### SUSPECT

The component has multiple abnormal indicators and should receive
engineering attention.

#### REJECT

The component violates an explicit engineering limit.

------------------------------------------------------------------------

# 16. Example Decision

Suppose:

``` text
Position Score = 1.2
Drift Score    = 4.7
Forecast       = Above expected range
Engineering    = Within current limit
```

The system may classify:

``` text
Decision = SUSPECT
```

Reason:

``` text
High degradation + concerning forecast
```

If the current measured value actually violates the engineering limit:

``` text
Decision = REJECT
```

regardless of the statistical scores.

------------------------------------------------------------------------

# 17. Risk Score

For prioritization, PRAKASH-DS can combine the main indicators into a
simple risk score.

For example:

``` text
Risk Score =
Position Score
+
Drift Score
+
Forecast Risk
```

The exact weighting should remain simple and documented.

The risk score is primarily used for:

> **Prioritization, not rejection.**

The Max Heap stores these risk scores so the system can quickly retrieve
the highest-risk components.

------------------------------------------------------------------------

# 18. Risk Prioritization Using Max Heap

Suppose the system calculates:

``` text
C001 → 3.2
C002 → 8.7
C003 → 5.1
C004 → 9.4
C005 → 6.8
```

The Max Heap prioritizes:

``` text
C004 → 9.4
C002 → 8.7
C005 → 6.8
C003 → 5.1
C001 → 3.2
```

The engineer can therefore inspect the most concerning components first.

------------------------------------------------------------------------

# 19. Component Search

Two different search mechanisms can be demonstrated.

## Hash Table Search

Used for direct lookup:

``` text
Search C104
       ↓
Hash Function
       ↓
Hash Table
       ↓
Component C104
```

Average complexity:

``` text
O(1)
```

## BST Search

Used when components are maintained in ordered form.

Example:

``` text
Search Risk Score / Component ID
        ↓
Compare with current node
        ↓
Left or Right
        ↓
Continue
```

Average complexity:

``` text
O(log n)
```

------------------------------------------------------------------------

# 20. Sorting

Sorting can be used to:

-   calculate median,
-   display components in order,
-   generate reports,
-   compare risk levels.

For example, a simple **Merge Sort** can be implemented.

Complexity:

``` text
O(n log n)
```

This is especially useful for demonstrating that sorting is part of the
analytical pipeline.

------------------------------------------------------------------------

# 21. Complete System Pipeline

The complete PRAKASH-DS pipeline is:

``` text
                 DATASET
                    |
                    v
          +-------------------+
          | Store Components  |
          +-------------------+
                    |
                    v
          +-------------------+
          | Arrays + Structs  |
          +-------------------+
                    |
                    v
          +-------------------+
          | Organize by Lot   |
          +-------------------+
                    |
                    v
          +-------------------+
          | Calculate Median  |
          | and MAD           |
          +-------------------+
                    |
                    v
          +-------------------+
          | Position Analysis |
          +-------------------+
                    |
                    v
          +-------------------+
          | Drift Analysis    |
          +-------------------+
                    |
                    v
          +-------------------+
          | 168h Forecast     |
          +-------------------+
                    |
                    v
          +-------------------+
          | Limit Check       |
          +-------------------+
                    |
                    v
          +-------------------+
          | Decision          |
          +-------------------+
                    |
                    v
          +-------------------+
          | Risk Score        |
          +-------------------+
                    |
                    v
          +-------------------+
          | Max Heap          |
          +-------------------+
                    |
                    v
            TOP RISK LIST
```

------------------------------------------------------------------------

# 22. PRAKASH-DS and Data Structures

## Why is this a Data Structures project?

This is the most important explanation for the Data Structures teacher.

PRAKASH-DS is not simply a project that happens to store data.

The **core workflow depends on Data Structures**.

### 22.1 Data Representation

Each component is represented using a C++ structure.

``` text
struct Component
```

This demonstrates:

-   structures,
-   records,
-   grouping related data.

### 22.2 Sequential Measurement Storage

The three timepoint measurements are stored using arrays.

``` text
leakage[3]
iddq[3]
```

This demonstrates:

-   arrays,
-   indexing,
-   direct access.

### 22.3 Dynamic Component Organization

Components in a lot are stored using a linked list.

``` text
C001 → C002 → C003 → NULL
```

This demonstrates:

-   nodes,
-   pointers,
-   dynamic memory,
-   insertion,
-   deletion,
-   traversal.

### 22.4 Fast Searching

The component ID is mapped to a component using a hash table.

``` text
Hash(Component ID)
        ↓
Table Index
        ↓
Component
```

This demonstrates:

-   hashing,
-   collision handling,
-   efficient searching.

### 22.5 Ordered Searching

A Binary Search Tree can organize components by ID or risk score.

This demonstrates:

-   trees,
-   recursion,
-   ordered insertion,
-   tree traversal,
-   searching.

### 22.6 Priority-Based Processing

The Max Heap stores risk scores.

The highest-risk component remains at the root.

This demonstrates:

-   heaps,
-   priority queues,
-   heap insertion,
-   heap deletion,
-   priority-based processing.

### 22.7 Algorithms

The project also uses standard DSA algorithms:

-   searching,
-   sorting,
-   traversal,
-   insertion,
-   deletion,
-   hashing,
-   heap operations,
-   tree operations.

Therefore:

> **The project uses Data Structures not merely to store the final data,
> but to implement the actual operations required by the screening
> system.**

------------------------------------------------------------------------

# 23. Simple Explanation for the Teacher

If the teacher asks:

> **"How is this project related to Data Structures?"**

A suitable answer is:

> "Sir/Ma'am, PRAKASH-DS is a burn-in screening system where Data
> Structures are directly used in the processing pipeline. Arrays store
> the measurements at different timepoints, structures represent
> individual components, linked lists organize components dynamically
> within lots, hash tables provide fast component-ID searching, a binary
> search tree provides ordered searching and traversal, and a max heap
> prioritizes the highest-risk components. On top of these structures,
> we implement algorithms such as sorting, searching, traversal, and
> statistical calculations. So the Data Structures are not added just
> for demonstration; each one solves a specific problem in the
> application."

### Shorter viva answer

> **"PRAKASH-DS uses arrays for measurements, structures for component
> records, linked lists for lot-wise storage, hash tables for fast
> lookup, BSTs for ordered searching, and a max heap for risk
> prioritization. These structures form the computational backbone of
> the burn-in screening system."**

------------------------------------------------------------------------

# 24. Why Not Use Every Data Structure?

Not every Data Structure needs to be forced into the project.

For example:

### Stack

A stack does not naturally represent the core burn-in workflow.

### Graph

A graph is not required because the project does not primarily model
relationships between interconnected entities.

### Queue

A queue could be added for incoming measurements, but it is not
necessary for the first version.

The project therefore focuses on structures that have a clear practical
purpose.

This makes the project easier to:

-   implement,
-   understand,
-   explain,
-   debug,
-   defend in viva.

------------------------------------------------------------------------

# 25. Complexity Reduction From PRAKASH 1.2

The original PRAKASH system contains advanced statistical and ML
components.

PRAKASH-DS removes or simplifies the following:

``` text
MCD
Mahalanobis distance
Isolation Forest
Huber regression
Conditional Δ-PAT
Power-law optimization
Conformal prediction
Complex ML validation
Ablation framework
Generator-shift ML evaluation
SHAP / explainability models
```

These are not required to demonstrate the Data Structures concepts.

Instead, PRAKASH-DS uses:

``` text
Median
MAD
Difference
Simple deviation score
Linear forecast
Decision rules
Risk score
```

This makes the system substantially easier to implement in C++.

------------------------------------------------------------------------

# 26. What Is Preserved From PRAKASH 1.2?

The simplified project intentionally preserves:

-   burn-in screening,
-   component-level analysis,
-   lot-based comparison,
-   Leakage and IDDQ,
-   early measurements,
-   degradation detection,
-   future-value prediction,
-   engineering-limit checking,
-   evidence-based decisions,
-   PASS/MONITOR/SUSPECT/REJECT,
-   risk prioritization,
-   the rule that statistical screening does not independently reject a
    component.

Thus:

> **PRAKASH-DS simplifies the implementation, not the core problem.**

------------------------------------------------------------------------

# 27. C++ Implementation Plan

The project should preferably be implemented in C++ because it is a Data
Structures course project.

A clean implementation can be divided into modules.

``` text
PRAKASH-DS/
│
├── main.cpp
├── component.h
├── component.cpp
│
├── linkedlist.h
├── linkedlist.cpp
│
├── hashtable.h
├── hashtable.cpp
│
├── bst.h
├── bst.cpp
│
├── maxheap.h
├── maxheap.cpp
│
├── statistics.h
├── statistics.cpp
│
├── screening.h
├── screening.cpp
│
├── dataset.csv
└── README.md
```

------------------------------------------------------------------------

# 28. Suggested C++ Modules

## `component`

Defines the component structure.

Responsible for:

-   component ID,
-   lot ID,
-   measurements,
-   scores,
-   decision.

## `linkedlist`

Responsible for:

-   node creation,
-   insertion,
-   deletion,
-   traversal.

## `hashtable`

Responsible for:

-   hashing,
-   insertion,
-   searching,
-   collision handling.

## `bst`

Responsible for:

-   insertion,
-   search,
-   traversal.

## `maxheap`

Responsible for:

-   risk insertion,
-   maximum retrieval,
-   extraction.

## `statistics`

Responsible for:

-   median,
-   MAD,
-   drift,
-   deviation scores.

## `screening`

Responsible for:

-   position analysis,
-   drift analysis,
-   forecasting,
-   limit checks,
-   final decision.

------------------------------------------------------------------------

# 29. User Interface

The first version can be a simple console application.

``` text
========================================
             PRAKASH-DS
   Burn-In Component Screening System
========================================

1. Load Dataset
2. Display Lot
3. Search Component
4. Analyze Component
5. Show Top Risk Components
6. Display Statistics
7. Display Decision Report
8. Exit

Enter choice:
```

This keeps the project focused on Data Structures instead of spending
excessive time on UI.

------------------------------------------------------------------------

# 30. Example Component Analysis

``` text
========================================
       COMPONENT ANALYSIS
========================================

Component ID : C104
Lot          : LOT-A

LEAKAGE CURRENT
0h    : 10.2 uA
24h   : 13.8 uA
168h  : 42.7 uA

IDDQ
0h    : 3.1 uA
24h   : 3.6 uA
168h  : 7.4 uA

----------------------------------------
Position Score : 2.1
Drift Score    : 4.7
Forecast       : 61.2

Risk Score     : 8.9

Decision       : SUSPECT

Reason:
High drift and concerning future trend.
========================================
```

------------------------------------------------------------------------

# 31. Example Search

User enters:

``` text
Search Component ID: C104
```

The hash table performs:

``` text
C104
 ↓
Hash Function
 ↓
Index
 ↓
Component Record
```

The system displays the component.

------------------------------------------------------------------------

# 32. Example Top-Risk Report

``` text
========================================
           TOP RISK COMPONENTS
========================================

Rank   Component   Risk Score   Decision

1      C104        9.4          SUSPECT
2      C071        8.8          SUSPECT
3      C231        8.2          MONITOR
4      C155        7.9          MONITOR
5      C019        7.5          MONITOR

========================================
```

The Max Heap is responsible for prioritizing these components.

------------------------------------------------------------------------

# 33. Complexity Analysis

  Operation                          Data Structure / Algorithm           Complexity
  ---------------------------------- ---------------------------- ------------------
  Array access                       Array                                      O(1)
  Add linked-list node at head       Linked List                                O(1)
  Linked-list traversal              Linked List                                O(n)
  Hash search                        Hash Table                         O(1) average
  Hash search worst case             Hash Table                                 O(n)
  BST search                         BST                            O(log n) average
  BST search worst case              BST                                        O(n)
  Merge Sort                         Sorting                              O(n log n)
  Median calculation after sorting   Sorting                              O(n log n)
  Drift calculation                  Array traversal                            O(n)
  Forecast                           Arithmetic                                 O(1)
  Heap insertion                     Max Heap                               O(log n)
  Get maximum risk                   Max Heap                                   O(1)
  Remove maximum                     Max Heap                               O(log n)

------------------------------------------------------------------------

# 34. Correctness and Validation

The project should be tested using known examples.

## Test 1 --- Normal Component

Expected:

``` text
PASS
```

## Test 2 --- One Abnormal Indicator

Expected:

``` text
MONITOR
```

## Test 3 --- Multiple Abnormal Indicators

Expected:

``` text
SUSPECT
```

## Test 4 --- Engineering Limit Violation

Expected:

``` text
REJECT
```

## Test 5 --- Hash Search

Search for an existing component.

Expected:

``` text
Component Found
```

## Test 6 --- Missing Component

Search for a non-existing ID.

Expected:

``` text
Component Not Found
```

## Test 7 --- Max Heap

Insert several risk scores.

Expected:

``` text
Highest score appears first.
```

## Test 8 --- BST

Insert component IDs and perform:

``` text
Inorder Traversal
```

Expected:

``` text
Sorted order
```

------------------------------------------------------------------------

# 35. Important Design Principle

The project must clearly distinguish between:

### Engineering Decision

``` text
Engineering limit violated
        ↓
REJECT
```

and:

### Statistical / Algorithmic Risk

``` text
Unusual position
        +
High drift
        +
Concerning forecast
        ↓
SUSPECT
```

The statistical calculations are intended to prioritize investigation.

They are not presented as certified qualification rules.

------------------------------------------------------------------------

# 36. Limitations

PRAKASH-DS is an academic prototype.

It should not be presented as:

-   a certified reliability system,
-   a production semiconductor qualification system,
-   an aerospace-qualified system,
-   a replacement for real engineering qualification procedures.

The dataset may also be synthetic or educational.

Therefore, the project demonstrates:

> **algorithmic screening and Data Structures implementation**, not
> industrial certification.

------------------------------------------------------------------------

# 37. Future Scope

Possible future extensions include:

1.  Four timepoints instead of three.
2.  Queue-based real-time measurement ingestion.
3.  Stack-based operation history/undo.
4.  Graph-based relationships between parameters.
5.  Balanced BST such as AVL Tree.
6.  More electrical parameters.
7.  More sophisticated statistical methods.
8.  Machine-learning anomaly detection.
9.  Graphical dashboard.
10. Database integration.
11. Real hardware/test-bench data integration.
12. Advanced reliability models.

These are future extensions and are not required for the core DS
project.

------------------------------------------------------------------------

# 38. Recommended Project Scope

The first working version should contain exactly:

### Data Structures

-   Array
-   Structure
-   Linked List
-   Hash Table
-   Binary Search Tree
-   Max Heap

### Algorithms

-   Median
-   MAD
-   Sorting
-   Searching
-   Drift calculation
-   Linear forecast
-   Risk scoring
-   Decision logic

### Application Features

-   Dataset loading
-   Lot display
-   Component search
-   Component analysis
-   Risk prioritization
-   Decision report

This is enough to create a complete and defendable Data Structures
project without unnecessarily increasing complexity.

------------------------------------------------------------------------

# 39. One-Line Project Definition

> **PRAKASH-DS is a C++ Data Structures-based burn-in screening system
> that stores, searches, analyzes, forecasts, and prioritizes electronic
> components using arrays, structures, linked lists, hash tables, binary
> search trees, and max heaps.**

------------------------------------------------------------------------

# 40. Final Concept

The complete idea can be remembered as:

``` text
STORE
  ↓
SEARCH
  ↓
COMPARE
  ↓
DETECT
  ↓
FORECAST
  ↓
DECIDE
  ↓
PRIORITIZE
```

Using:

``` text
ARRAY
STRUCTURE
LINKED LIST
HASH TABLE
BST
MAX HEAP
```

The project therefore combines a real engineering problem with the core
concepts taught in a Data Structures course.

------------------------------------------------------------------------

# 41. Viva Cheat Sheet

### What is PRAKASH-DS?

A simplified Data Structures-based version of PRAKASH for burn-in
component screening.

### What problem does it solve?

It identifies components that are abnormal, degrading unusually fast, or
likely to become problematic by 168 hours.

### Why Data Structures?

Because the system needs to store, organize, search, order, and
prioritize large numbers of component records.

### Why arrays?

For fixed timepoint measurements.

### Why structures?

To group all information belonging to one component.

### Why linked list?

For dynamic lot-wise component storage.

### Why hash table?

For fast component-ID lookup.

### Why BST?

For ordered searching and traversal.

### Why max heap?

To prioritize the highest-risk components.

### Why did you simplify PRAKASH 1.2?

Because the original system contains advanced statistical and ML methods
that are unnecessary for demonstrating core Data Structures. The
simplified system keeps the main screening behavior while making the
implementation appropriate for a DS course.

### What makes this different from a normal DS demo?

The Data Structures solve actual application problems instead of being
separate textbook demonstrations.

### Can the system reject a component because of a statistical score?

No. REJECT is reserved for an explicit engineering-limit violation.
Statistical analysis produces monitoring/suspicion/risk information.

------------------------------------------------------------------------

# 42. Final Architecture

``` text
                         PRAKASH-DS
                              |
              +---------------+---------------+
              |                               |
          DATA LAYER                     ANALYSIS
              |                               |
       +------+------+              +---------+---------+
       |      |      |              |         |         |
     Array  Struct  List         Position   Drift   Forecast
       |      |      |              |         |         |
       +------+------+              +---------+---------+
              |                               |
              v                               v
          Hash Table                       Decision
              |                               |
              v                               v
             BST                         Risk Score
              |                               |
              +---------------+---------------+
                              |
                              v
                          Max Heap
                              |
                              v
                     TOP RISK COMPONENTS
```

------------------------------------------------------------------------

# 43. Project Philosophy

The project follows one central principle:

> **Keep the engineering problem meaningful, but keep the implementation
> simple enough that every Data Structure and algorithm can be
> understood and defended by the student who built it.**

PRAKASH-DS is therefore intentionally smaller than PRAKASH 1.2, but it
preserves the central screening workflow:

``` text
Component Measurements
        ↓
Lot Comparison
        ↓
Abnormality Detection
        ↓
Drift Detection
        ↓
Future Forecast
        ↓
Engineering Check
        ↓
PASS / MONITOR / SUSPECT / REJECT
        ↓
Risk Prioritization
```

**End of PRAKASH-DS Specification**
