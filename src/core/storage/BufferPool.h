//
// Created by root on 7/27/25.
//

#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include <list>
#include <unordered_map>

#include "Page.h"

class BufferPool {

    struct LinkedNode {
        LinkedNode* prev;
        LinkedNode* next;
        std::string key;
        Page*       val;
    };

    size_t capacity;
    std::unordered_map<std::string, LinkedNode*> identifierToNode;

    LinkedNode* head;
    LinkedNode* back;

    void moveNodeToFront(LinkedNode* node);
    void removeLRU();

    public:

    BufferPool(size_t capacity);
    ~BufferPool();

    Page* getPage(const std::string &file, int num);


};



#endif //BUFFERPOOL_H
