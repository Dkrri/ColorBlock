#include <iostream>
#include "AStarNode.h"

int main() {
    AStarNode node;
    if (node.state != nullptr) return 1;
    if (node.parent != nullptr) return 1;
    if (node.moveFromParent.blockId != -1) return 1;
    if (node.g != 0 || node.h != 0 || node.f != 0) return 1;

    std::cout << "test_astarnode OK\n";
    return 0;
}
