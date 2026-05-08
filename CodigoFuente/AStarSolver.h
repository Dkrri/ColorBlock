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

private:
    static int absInt(int value);
    static int gcdInt(int a, int b);
    static int lcmCapped(int a, int b, int cap);
    static int computeTemporalPeriod(const Board& board);
    static unsigned long makeTemporalKey(const GameState& state, int temporalPeriod);
    static int cellDistanceLowerBound(const GameState& state, const Block& block);
    static int alignmentDistanceLowerBound(const GameState& state, const Block& block);
    static int blockHeuristic(const GameState& state, const Block& block);
    static int heuristic(const GameState& state);
};
