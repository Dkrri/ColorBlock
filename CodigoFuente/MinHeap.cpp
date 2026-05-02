#include "MinHeap.h"

MinHeap::MinHeap()
    : data(nullptr), count(0), capacity(0) {}

MinHeap::~MinHeap() {
    delete[] data;
}

bool MinHeap::lessNode(const AStarNode* a, const AStarNode* b) const {
    if (a->f != b->f) return a->f < b->f;
    if (a->h != b->h) return a->h < b->h;
    return a->g > b->g;
}

void MinHeap::ensureCapacity(int minCapacity) {
    if (capacity >= minCapacity) return;

    int newCapacity = (capacity == 0) ? 256 : capacity;
    while (newCapacity < minCapacity) {
        newCapacity *= 2;
    }

    AStarNode** newData = new AStarNode*[newCapacity];
    for (int i = 0; i < count; ++i) {
        newData[i] = data[i];
    }

    delete[] data;
    data = newData;
    capacity = newCapacity;
}

void MinHeap::push(AStarNode* node) {
    ensureCapacity(count + 1);

    data[count] = node;
    int index = count;
    ++count;

    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!lessNode(data[index], data[parent])) break;

        AStarNode* temp = data[parent];
        data[parent] = data[index];
        data[index] = temp;
        index = parent;
    }
}

AStarNode* MinHeap::pop() {
    if (count == 0) return nullptr;

    AStarNode* root = data[0];
    --count;

    if (count > 0) {
        data[0] = data[count];
        int index = 0;

        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int best = index;

            if (left < count && lessNode(data[left], data[best])) {
                best = left;
            }
            if (right < count && lessNode(data[right], data[best])) {
                best = right;
            }
            if (best == index) break;

            AStarNode* temp = data[index];
            data[index] = data[best];
            data[best] = temp;
            index = best;
        }
    }

    return root;
}

bool MinHeap::empty() const {
    return count == 0;
}

int MinHeap::size() const {
    return count;
}
