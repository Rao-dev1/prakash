#ifndef MAXHEAP_H
#define MAXHEAP_H

#include "component.h"
#include <vector>

// Max Heap Priority Queue for prioritizing high-risk components
// Complete binary tree stored in a sequential dynamic array
class ComponentMaxHeap {
private:
    Component** heap;
    int capacity;
    int count;

    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }

    void heapifyUp(int index);
    void heapifyDown(int index);
    void resize(int newCapacity);

public:
    ComponentMaxHeap(int initialCapacity = 64);
    ~ComponentMaxHeap();

    // Insertion: O(log n)
    void insert(Component* comp);

    // Get highest-risk component: O(1)
    Component* peekMax() const;

    // Remove and return highest-risk component: O(log n)
    Component* extractMax();

    int size() const;
    bool isEmpty() const;
    void clear();

    // Retrieves top K highest risk components without mutating the heap
    std::vector<Component*> getTopK(int k) const;

    // Expose internal array representation for heap tree visualization in UI
    struct HeapItem {
        int index;
        std::string componentID;
        std::string lotID;
        double riskScore;
        std::string decision;
        int parentIndex;
        int leftIndex;
        int rightIndex;
    };
    std::vector<HeapItem> getHeapStructure(int maxItems = 63) const;
};

#endif // MAXHEAP_H
