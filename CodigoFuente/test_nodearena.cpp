#include <iostream>
#include "NodeArena.h"

int main() {
    Board board(5, 5);
    bool geometry[1] = {true};
    Block blocks[1];
    blocks[0] = Block(1, 0, 'a', 1, 1, 2, 2, geometry);
    GameState state(board, blocks, 1);
    GameState::Move move = {1, GameState::DIR_RIGHT, 1};

    NodeArena arena;
    AStarNode* node = arena.create(state, nullptr, move, 2, 3);
    if (node == nullptr) return 1;
    if (node->f != 5) return 1;
    if (arena.size() != 1) return 1;
    arena.clear();
    if (arena.size() != 0) return 1;

    std::cout << "test_nodearena OK\n";
    return 0;
}
