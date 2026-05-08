#include <iostream>
#include "AStarSolver.h"
#include "FileParser.h"

int main() {
    FileParser::ParsedLevel level;
    if (!FileParser::loadLevel("simple1.txt", level)) return 1;

    GameState initialState(*level.board, level.blocks, level.blockCount);
    AStarSolver::Result result = AStarSolver::solve(initialState, 10000, level.stepLimit);

    if (!result.found) return 1;
    if (result.moveCount != 1) return 1;
    if (result.moves[0].blockId != 1) return 1;
    if (result.moves[0].direction != GameState::DIR_RIGHT) return 1;
    if (result.moves[0].distance != 1) return 1;

    AStarSolver::freeResult(result);
    FileParser::freeLevel(level);
    std::cout << "test_astarsolver OK\n";
    return 0;
}
