#include <iostream>
#include "MinHeap.h"

int main() {
    MinHeap heap;
    AStarNode a;
    AStarNode b;
    AStarNode c;

    a.f = 5; a.h = 2; a.g = 3;
    b.f = 2; b.h = 1; b.g = 1;
    c.f = 5; c.h = 1; c.g = 4;

    heap.push(&a);
    heap.push(&b);
    heap.push(&c);

    if (heap.size() != 3) return 1;
    if (heap.pop() != &b) return 1;
    if (heap.pop() != &c) return 1;
    if (heap.pop() != &a) return 1;
    if (!heap.empty()) return 1;

    std::cout << "test_minheap OK\n";
    return 0;
}
