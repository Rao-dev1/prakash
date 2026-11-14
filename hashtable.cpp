#include "hashtable.h"
#include <algorithm>

HashNode::HashNode(const std::string& k, Component* v)
    : key(k), value(v), next(nullptr) {}

ComponentHashTable::ComponentHashTable() : totalElements(0) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        table[i] = nullptr;
    }
}

ComponentHashTable::~ComponentHashTable() {
    clear();
}

// Polynomial rolling hash function
unsigned long ComponentHashTable::hashFunction(const std::string& key) const {
    unsigned long hash = 0;
    const unsigned long p = 31;
    for (char c : key) {
        hash = (hash * p + static_cast<unsigned char>(c)) % TABLE_SIZE;
    }
    return hash;
}

void ComponentHashTable::insert(const std::string& key, Component* value) {
    unsigned long index = hashFunction(key);
    HashNode* curr = table[index];

    // If key already exists in chain, update the value
    while (curr) {
        if (curr->key == key) {
            curr->value = value;
            return;
        }
        curr = curr->next;
    }

    // Insert new node at head of chain (O(1))
    HashNode* newNode = new HashNode(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    totalElements++;
}

Component* ComponentHashTable::search(const std::string& key) const {
    unsigned long index = hashFunction(key);
    HashNode* curr = table[index];
    while (curr) {
        if (curr->key == key) {
            return curr->value;
        }
        curr = curr->next;
    }
    return nullptr;
}

bool ComponentHashTable::remove(const std::string& key) {
    unsigned long index = hashFunction(key);
    HashNode* curr = table[index];
    HashNode* prev = nullptr;

    while (curr) {
        if (curr->key == key) {
            if (prev) {
                prev->next = curr->next;
            } else {
                table[index] = curr->next;
            }
            delete curr;
            totalElements--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

int ComponentHashTable::size() const {
    return totalElements;
}

void ComponentHashTable::clear() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        HashNode* curr = table[i];
        while (curr) {
            HashNode* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
        table[i] = nullptr;
    }
    totalElements = 0;
}

int ComponentHashTable::getOccupiedBucketCount() const {
    int count = 0;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        if (table[i] != nullptr) {
            count++;
        }
    }
    return count;
}

int ComponentHashTable::getCollisionCount() const {
    int collisions = 0;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        HashNode* curr = table[i];
        if (curr && curr->next) {
            int chainLen = 0;
            while (curr) {
                chainLen++;
                curr = curr->next;
            }
            collisions += (chainLen - 1);
        }
    }
    return collisions;
}

int ComponentHashTable::getMaxChainLength() const {
    int maxLen = 0;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        int len = 0;
        HashNode* curr = table[i];
        while (curr) {
            len++;
            curr = curr->next;
        }
        if (len > maxLen) {
            maxLen = len;
        }
    }
    return maxLen;
}

double ComponentHashTable::getLoadFactor() const {
    return static_cast<double>(totalElements) / TABLE_SIZE;
}

std::vector<ComponentHashTable::BucketInfo> ComponentHashTable::getActiveBuckets(int maxBucketsToReturn) const {
    std::vector<BucketInfo> result;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        if (table[i] != nullptr) {
            BucketInfo info;
            info.bucketIndex = i;
            HashNode* curr = table[i];
            while (curr) {
                info.keys.push_back(curr->key);
                curr = curr->next;
            }
            result.push_back(info);
            if (static_cast<int>(result.size()) >= maxBucketsToReturn) {
                break;
            }
        }
    }
    return result;
}
