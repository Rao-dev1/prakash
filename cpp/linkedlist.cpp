#include "linkedlist.h"

ComponentNode::ComponentNode(Component* comp) 
    : component(comp), next(nullptr) {}

LotLinkedList::LotLinkedList(const std::string& lot)
    : lotID(lot), head(nullptr), tail(nullptr), count(0) {}

LotLinkedList::~LotLinkedList() {
    clear();
}

void LotLinkedList::setLotID(const std::string& lot) {
    lotID = lot;
}

std::string LotLinkedList::getLotID() const {
    return lotID;
}

void LotLinkedList::insert(Component* comp) {
    if (!comp) return;

    ComponentNode* newNode = new ComponentNode(comp);
    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

ComponentNode* LotLinkedList::getHead() const {
    return head;
}

int LotLinkedList::size() const {
    return count;
}

std::vector<Component*> LotLinkedList::toVector() const {
    std::vector<Component*> list;
    list.reserve(count);
    ComponentNode* curr = head;
    while (curr) {
        if (curr->component) {
            list.push_back(curr->component);
        }
        curr = curr->next;
    }
    return list;
}

void LotLinkedList::clear() {
    ComponentNode* curr = head;
    while (curr) {
        ComponentNode* nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
    head = tail = nullptr;
    count = 0;
}

// LotNode implementation
LotNode::LotNode(const std::string& id) 
    : lotID(id), componentList(id), next(nullptr) {}

// LotDirectory implementation
LotDirectory::LotDirectory() : head(nullptr), lotCount(0) {}

LotDirectory::~LotDirectory() {
    clear();
}

LotLinkedList* LotDirectory::getOrCreateLot(const std::string& lotID) {
    LotLinkedList* existing = findLot(lotID);
    if (existing) return existing;

    LotNode* newNode = new LotNode(lotID);
    newNode->next = head;
    head = newNode;
    lotCount++;
    return &(newNode->componentList);
}

LotLinkedList* LotDirectory::findLot(const std::string& lotID) const {
    LotNode* curr = head;
    while (curr) {
        if (curr->lotID == lotID) {
            return const_cast<LotLinkedList*>(&(curr->componentList));
        }
        curr = curr->next;
    }
    return nullptr;
}

LotNode* LotDirectory::getHead() const {
    return head;
}

int LotDirectory::size() const {
    return lotCount;
}

void LotDirectory::clear() {
    LotNode* curr = head;
    while (curr) {
        LotNode* nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
    head = nullptr;
    lotCount = 0;
}
