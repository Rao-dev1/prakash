#include "maxheap.h"
#include <algorithm>
#include <utility>

ComponentMaxHeap::ComponentMaxHeap(int initialCapacity) 
    : capacity(initialCapacity > 4 ? initialCapacity : 4), count(0) 
{
    heap = new Component*[capacity];
}

ComponentMaxHeap::~ComponentMaxHeap() {
    delete[] heap;
}

void ComponentMaxHeap::resize(int newCapacity) {
    Component** newHeap = new Component*[newCapacity];
    for (int i = 0; i < count; ++i) {
        newHeap[i] = heap[i];
    }
    delete[] heap;
    heap = newHeap;
    capacity = newCapacity;
}

void ComponentMaxHeap::heapifyUp(int index) {
    while (index > 0 && heap[parent(index)]->riskScore < heap[index]->riskScore) {
        std::swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}

void ComponentMaxHeap::heapifyDown(int index) {
    int maxIndex = index;
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < count && heap[left]->riskScore > heap[maxIndex]->riskScore) {
        maxIndex = left;
    }
    if (right < count && heap[right]->riskScore > heap[maxIndex]->riskScore) {
        maxIndex = right;
    }

    if (index != maxIndex) {
        std::swap(heap[index], heap[maxIndex]);
        heapifyDown(maxIndex);
    }
}

void ComponentMaxHeap::insert(Component* comp) {
    if (!comp) return;

    if (count == capacity) {
        resize(capacity * 2);
    }

    heap[count] = comp;
    heapifyUp(count);
    count++;
}

Component* ComponentMaxHeap::peekMax() const {
    if (count == 0) return nullptr;
    return heap[0];
}

Component* ComponentMaxHeap::extractMax() {
    if (count == 0) return nullptr;

    Component* maxItem = heap[0];
    heap[0] = heap[count - 1];
    count--;

    if (count > 0) {
        heapifyDown(0);
    }
    return maxItem;
}

int ComponentMaxHeap::size() const {
    return count;
}

bool ComponentMaxHeap::isEmpty() const {
    return count == 0;
}

void ComponentMaxHeap::clear() {
    count = 0;
}

std::vector<Component*> ComponentMaxHeap::getTopK(int k) const {
    std::vector<Component*> result;
    if (count == 0 || k <= 0) return result;

    // Use a temporary Max Heap copy so original is preserved
    ComponentMaxHeap tempHeap(count);
    for (int i = 0; i < count; ++i) {
        tempHeap.insert(heap[i]);
    }

    int limit = std::min(k, count);
    for (int i = 0; i < limit; ++i) {
        result.push_back(tempHeap.extractMax());
    }
    return result;
}

std::vector<ComponentMaxHeap::HeapItem> ComponentMaxHeap::getHeapStructure(int maxItems) const {
    std::vector<HeapItem> items;
    int limit = std::min(count, maxItems);
    items.reserve(limit);

    for (int i = 0; i < limit; ++i) {
        HeapItem it;
        it.index = i;
        it.componentID = heap[i]->componentID;
        it.lotID = heap[i]->lotID;
        it.riskScore = heap[i]->riskScore;
        it.decision = heap[i]->decision;
        it.parentIndex = (i == 0) ? -1 : parent(i);
        it.leftIndex = (leftChild(i) < count) ? leftChild(i) : -1;
        it.rightIndex = (rightChild(i) < count) ? rightChild(i) : -1;
        items.push_back(it);
    }
    return items;
}
