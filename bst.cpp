#include "bst.h"
#include <algorithm>

BSTNode::BSTNode(Component* comp) 
    : data(comp), left(nullptr), right(nullptr) {}

ComponentBST::ComponentBST() : root(nullptr), nodeCount(0) {}

ComponentBST::~ComponentBST() {
    clear();
}

BSTNode* ComponentBST::insertHelper(BSTNode* node, Component* comp) {
    if (!node) {
        nodeCount++;
        return new BSTNode(comp);
    }

    if (comp->componentID < node->data->componentID) {
        node->left = insertHelper(node->left, comp);
    } else if (comp->componentID > node->data->componentID) {
        node->right = insertHelper(node->right, comp);
    } else {
        // ID matches, update pointer
        node->data = comp;
    }
    return node;
}

void ComponentBST::insert(Component* comp) {
    if (!comp) return;
    root = insertHelper(root, comp);
}

BSTNode* ComponentBST::searchHelper(BSTNode* node, const std::string& id) const {
    if (!node || node->data->componentID == id) {
        return node;
    }

    if (id < node->data->componentID) {
        return searchHelper(node->left, id);
    } else {
        return searchHelper(node->right, id);
    }
}

Component* ComponentBST::search(const std::string& id) const {
    BSTNode* res = searchHelper(root, id);
    return res ? res->data : nullptr;
}

void ComponentBST::inorderHelper(BSTNode* node, std::vector<Component*>& result) const {
    if (!node) return;
    inorderHelper(node->left, result);
    if (node->data) result.push_back(node->data);
    inorderHelper(node->right, result);
}

std::vector<Component*> ComponentBST::getInorder() const {
    std::vector<Component*> result;
    result.reserve(nodeCount);
    inorderHelper(root, result);
    return result;
}

int ComponentBST::heightHelper(BSTNode* node) const {
    if (!node) return 0;
    return 1 + std::max(heightHelper(node->left), heightHelper(node->right));
}

int ComponentBST::getHeight() const {
    return heightHelper(root);
}

void ComponentBST::clearHelper(BSTNode* node) {
    if (!node) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void ComponentBST::clear() {
    clearHelper(root);
    root = nullptr;
    nodeCount = 0;
}

int ComponentBST::size() const {
    return nodeCount;
}
