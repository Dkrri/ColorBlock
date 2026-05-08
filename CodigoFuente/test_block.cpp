#include <iostream>
#include "Block.h"

int main() {
    bool geometry[4] = {true, false, false, true};
    Block block(1, 0, 'a', 2, 2, 3, 4, geometry);
    Block other(2, 0, 'b', 1, 1, 4, 5, nullptr);

    if (!block.occupiesCell(3, 4)) return 1;
    if (block.occupiesCell(3, 5)) return 1;
    if (!block.occupiesCell(4, 5)) return 1;
    if (!block.collidesWith(other)) return 1;

    block.setX(1);
    block.setY(2);
    if (block.getX() != 1 || block.getY() != 2) return 1;

    std::cout << "test_block OK\n";
    return 0;
}
