#include <iostream>
#include "Board.h"

int main() {
    Board board(5, 5);
    Wall wall(1, 1);
    bool geometry[4] = {true, true, true, true};
    Block block(1, 0, 'a', 2, 2, 2, 2, geometry);

    if (!board.addWall(wall)) return 1;
    if (!board.isWallAt(1, 1)) return 1;
    if (!board.canPlaceBlock(block)) return 1;
    if (!board.placeBlock(block)) return 1;
    if (board.isCellFree(2, 2)) return 1;

    board.removeBlock(block);
    if (!board.isCellFree(2, 2)) return 1;

    std::cout << "test_board OK\n";
    return 0;
}
