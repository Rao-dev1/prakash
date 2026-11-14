#ifndef BST_H
#define BST_H

#include "component.h"
#include <string>
#include <vector>

// Node for Binary Search Tree
struct BSTNode {
    Component* data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(Component* comp);
};

// Binary Search Tree organizing components by Component ID (ordered storage & traversal)
class ComponentBST {
private:
    BSTNode* root;
    int nodeCount;

    BSTNode* insertHelper(BSTNode* node, Component* comp);
    BSTNode* searchHelper(BSTNode* node, const std::string& id) const;
    void inorderHelper(BSTNode* node, std::vector<Component*>& result) const;
    int heightHelper(BSTNode* node) const;
    void clearHelper(BSTNode* node);

public:
    ComponentBST();
    ~ComponentBST();

    // Insertion: Average O(log n), Worst O(n)
    void insert(Component* comp);

    // Search: Average O(log n), Worst O(n)
    Component* search(const std::string& id) const;

    // In-order Traversal: Visits left-subtree, root, right-subtree.
    // Yields components strictly sorted by Component ID in O(n)
    std::vector<Component*> getInorder() const;

    int size() const;
    int getHeight() const;
    void clear();

    BSTNode* getRoot() const { return root; }
};

#endif // BST_H
