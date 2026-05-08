#include <iostream>
#include "FileParser.h"
#include "GameState.h"

int main() {
    FileParser::ParsedLevel level;
    if (!FileParser::loadLevel("simple1.txt", level)) return 1;

    GameState state(*level.board, level.blocks, level.blockCount);
    GameState::Move moves[16];
    int count = state.generateMovementsForBlock(1, moves, 16);
    if (count <= 0) return 1;

    GameState::Move move = {1, GameState::DIR_RIGHT, 1};
    if (!state.applyMove(move)) return 1;
    if (!state.isGoal()) return 1;
    if (state.getStep() != 1) return 1;

    FileParser::freeLevel(level);
    std::cout << "test_gamestate OK\n";
    return 0;
}
