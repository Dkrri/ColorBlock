#include <iostream>
#include "Wall.h"

int main() {
    Wall wall(2, 3, true, false);
    if (!wall.occupiesCell(2, 3)) return 1;
    if (wall.occupiesCell(3, 2)) return 1;
    if (!wall.isExitWall() || wall.isGateWall()) return 1;

    wall.setGateWall(true);
    wall.setPosition(4, 5);
    if (!wall.isGateWall()) return 1;
    if (!wall.occupiesCell(4, 5)) return 1;

    std::cout << "test_wall OK\n";
    return 0;
}
