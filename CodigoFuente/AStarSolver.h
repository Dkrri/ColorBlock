#pragma once

#include "GameState.h"

class AStarSolver {
public:
    struct Result {
        bool found;
        int moveCount;
        GameState::Move* moves;
        int expanded;
        int generated;
    };

    static Result solve(const GameState& start, int maxIterations, int stepLimit);
    static void freeResult(Result& result);
};
