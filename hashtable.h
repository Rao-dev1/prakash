#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "component.h"
#include <string>
#include <vector>

// Node for separate chaining in Hash Table
struct HashNode {
    std::string key;
    Component* value;
    HashNode* next;

    HashNode(const std::string& k, Component* v);
};

// Component Hash Table with separate chaining
class ComponentHashTable {
private:
    static const int TABLE_SIZE = 1009; // Prime table size for optimal distribution
    HashNode* table[TABLE_SIZE];
    int totalElements;

    // Polynomial rolling hash function: O(L) where L is string length
    unsigned long hashFunction(const std::string& key) const;

public:
    ComponentHashTable();
    ~ComponentHashTable();

    // Insertion: Average O(1)
    void insert(const std::string& key, Component* value);

    // Search: Average O(1)
    Component* search(const std::string& key) const;

    // Removal: Average O(1)
    bool remove(const std::string& key);

    int size() const;
    void clear();

    // Inspection metrics for Data Structures course defense
    int getTableCapacity() const { return TABLE_SIZE; }
    int getOccupiedBucketCount() const;
    int getCollisionCount() const;
    int getMaxChainLength() const;
    double getLoadFactor() const;

    // Export non-empty buckets for visual inspection in dashboard / CLI
    struct BucketInfo {
        int bucketIndex;
        std::vector<std::string> keys;
    };
    std::vector<BucketInfo> getActiveBuckets(int maxBucketsToReturn = 50) const;
};

#endif // HASHTABLE_H
