//
// Created by root on 7/27/25.
//

#include "BufferPool.h"

BufferPool::BufferPool(size_t capacity): capacity(capacity) {
    head = new LinkedNode(nullptr, nullptr, "", nullptr);
    back = new LinkedNode(nullptr, nullptr, "", nullptr);
    head->next = back;
    back->prev = head;
}

BufferPool::~BufferPool() {

    LinkedNode* ptr = head;

    while (ptr != nullptr) {
        ptr = ptr->next;
        delete ptr->prev;
        delete ptr->val;
    }

    delete back;
}

void BufferPool::moveNodeToFront(LinkedNode *node) {

    // Remove Node from current position
    node->prev->next = node->next;
    node->next->prev = node->prev;
    // Reconfigure Node
    node->prev = head;
    node->next = head->next;
    // Insert behind head
    node->prev->next = node;
    node->next->prev = node;

}

void BufferPool::removeLRU() {

    LinkedNode* ptr = back->prev;
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
    identifierToNode.erase(ptr->key);
    delete ptr;

}

Page* BufferPool::getPage(const std::string &file, const int num) {

    const std::string key = file + "::" + std::to_string(num);

    if (identifierToNode.contains(key)) {
        return identifierToNode[key]->val; // Key already exists, return page
    }

    if (identifierToNode.size() == capacity) removeLRU();

    // Create New Node for Page
    auto new_node = new LinkedNode(head, head->next, key, new Page(file, num));

    // Insert just behind the head, and add to the map
    new_node->prev->next = new_node;
    new_node->next->prev = new_node;
    identifierToNode[key] = new_node;

    return new_node->val;
}
