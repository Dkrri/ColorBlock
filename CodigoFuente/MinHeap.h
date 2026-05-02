#pragma once

#include "AStarNode.h"

class MinHeap {
private:
    AStarNode** data;
    int count;
    int capacity;

    bool lessNode(const AStarNode* a, const AStarNode* b) const;
    void ensureCapacity(int minCapacity);

public:
    MinHeap();
    ~MinHeap();

    void push(AStarNode* node);
    AStarNode* pop();

    bool empty() const;
    int size() const;
};
