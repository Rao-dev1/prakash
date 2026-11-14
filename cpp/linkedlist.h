#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "component.h"
#include <string>
#include <vector>

// Node for a dynamic singly linked list of components
struct ComponentNode {
    Component* component;
    ComponentNode* next;

    ComponentNode(Component* comp);
};

// Linked List to dynamically store all components belonging to a specific Lot
class LotLinkedList {
private:
    std::string lotID;
    ComponentNode* head;
    ComponentNode* tail;
    int count;

public:
    LotLinkedList(const std::string& lot = "");
    ~LotLinkedList();

    void setLotID(const std::string& lot);
    std::string getLotID() const;

    // Insertion at the end: O(1) using tail pointer
    void insert(Component* comp);

    ComponentNode* getHead() const;
    int size() const;

    // Convert list elements to vector for fast random access indexing if needed
    std::vector<Component*> toVector() const;

    void clear();
};

// Node for linking multiple lots in a higher-level Lot list
struct LotNode {
    std::string lotID;
    LotLinkedList componentList;
    LotNode* next;

    LotNode(const std::string& id);
};

// Linked list of Lots
class LotDirectory {
private:
    LotNode* head;
    int lotCount;

public:
    LotDirectory();
    ~LotDirectory();

    LotLinkedList* getOrCreateLot(const std::string& lotID);
    LotLinkedList* findLot(const std::string& lotID) const;
    LotNode* getHead() const;
    int size() const;
    void clear();
};

#endif // LINKEDLIST_H
