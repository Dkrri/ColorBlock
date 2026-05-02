#include "AStarSolver.h"

#include "AStarNode.h"
#include "BestCostTable.h"
#include "Board.h"
#include "MinHeap.h"
#include "NodeArena.h"

#include <limits>

static int absInt(int value) {
    return (value < 0) ? -value : value;
}

static int gcdInt(int a, int b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return (a == 0) ? 1 : a;
}

static int lcmCapped(int a, int b, int cap) {
    int g = gcdInt(a, b);
    long long part = (long long)(a / g) * (long long)b;
    if (part > cap) return cap;
    return (int)part;
}

static int computeTemporalPeriod(const Board& board) {
    // Cap keeps memory bounded even for large theoretical cycles.
    const int CAP = 4096;
    int period = 1;

    for (int i = 0; i < board.getExitCount(); ++i) {
        const Exit& ex = board.getExits()[i];
        int li = ex.getInitialLength();
        int lf = ex.getFinalLength();
        int step = ex.getStepChange();
        int diff = absInt(li - lf);
        if (step > 0 && diff > 0) {
            int p = step * 2 * diff;
            period = lcmCapped(period, p, CAP);
        }
    }

    for (int i = 0; i < board.getGateCount(); ++i) {
        const Gate& g = board.getGates()[i];
        int step = g.getStepChange();
        char ci = g.getInitialColor();
        char cf = g.getFinalColor();
        int range = (cf >= ci) ? (cf - ci + 1) : 1;
        if (step > 0 && range > 1) {
            int p = step * range;
            period = lcmCapped(period, p, CAP);
        }
    }

    if (period <= 0) period = 1;
    return period;
}

static unsigned long makeTemporalKey(const GameState& state, int temporalPeriod) {
    unsigned long base = state.hash();
    unsigned long step = (unsigned long)(state.getStep() % temporalPeriod);
    // mix state hash with step so time-dependent exits/gates are distinguished
    return (base * 1315423911ul) ^ (step + 0x9e3779b9ul + (base << 6) + (base >> 2));
}

static int cellDistanceLowerBound(const GameState& state, const Block& block) {
    const Board& board = state.getBoard();
    int best = std::numeric_limits<int>::max();

    for (int bx = block.getX(); bx < block.getX() + block.getHeight(); ++bx) {
        for (int by = block.getY(); by < block.getY() + block.getWidth(); ++by) {
            if (!block.occupiesCell(bx, by)) continue;

            for (int e = 0; e < board.getExitCount(); ++e) {
                const Exit& ex = board.getExits()[e];
                if (ex.getColor() != block.getColor()) continue;

                int len = ex.getLengthAtStep(state.getStep());
                if (len <= 0) continue;

                if (ex.getOrientation() == 'H') {
                    for (int exy = ex.getY(); exy < ex.getY() + len; ++exy) {
                        int d = absInt(bx - ex.getX()) + absInt(by - exy);
                        if (d < best) best = d;
                    }
                } else {
                    for (int exx = ex.getX(); exx < ex.getX() + len; ++exx) {
                        int d = absInt(bx - exx) + absInt(by - ex.getY());
                        if (d < best) best = d;
                    }
                }
            }
        }
    }

    return (best == std::numeric_limits<int>::max()) ? 0 : best;
}

static int alignmentDistanceLowerBound(const GameState& state, const Block& block) {
    const Board& board = state.getBoard();
    int best = std::numeric_limits<int>::max();

    for (int e = 0; e < board.getExitCount(); ++e) {
        const Exit& ex = board.getExits()[e];
        if (ex.getColor() != block.getColor()) continue;

        int len = ex.getLengthAtStep(state.getStep());
        if (len <= 0) continue;

        if (ex.getOrientation() == 'H') {
            if (block.getWidth() > len) continue;

            int targetXUp = ex.getX() - block.getHeight();
            int targetXDown = ex.getX() + 1;
            int dxUp = absInt(block.getX() - targetXUp);
            int dxDown = absInt(block.getX() - targetXDown);
            int dx = (dxUp < dxDown) ? dxUp : dxDown;
            int minY = ex.getY();
            int maxY = ex.getY() + len - block.getWidth();
            int dy = 0;
            if (block.getY() < minY) dy = minY - block.getY();
            else if (block.getY() > maxY) dy = block.getY() - maxY;
            int d = dx + dy;
            if (d < best) best = d;
        } else {
            if (block.getHeight() > len) continue;

            int targetYLeft = ex.getY() - block.getWidth();
            int targetYRight = ex.getY() + 1;
            int dyLeft = absInt(block.getY() - targetYLeft);
            int dyRight = absInt(block.getY() - targetYRight);
            int dy = (dyLeft < dyRight) ? dyLeft : dyRight;
            int minX = ex.getX();
            int maxX = ex.getX() + len - block.getHeight();
            int dx = 0;
            if (block.getX() < minX) dx = minX - block.getX();
            else if (block.getX() > maxX) dx = block.getX() - maxX;
            int d = dx + dy;
            if (d < best) best = d;
        }
    }

    return (best == std::numeric_limits<int>::max()) ? 0 : best;
}

static int blockHeuristic(const GameState& state, const Block& block) {
    int cellLB = cellDistanceLowerBound(state, block);
    int alignLB = alignmentDistanceLowerBound(state, block);
    return (cellLB > alignLB) ? cellLB : alignLB;
}

static int heuristic(const GameState& state) {
    int total = 0;

    for (int i = 0; i < state.getBlockCount(); ++i) {
        const Block& block = state.getBlock(i);
        if (state.hasBlockExited(block.getId())) continue;

        total += blockHeuristic(state, block);
    }

    return total;
}

AStarSolver::Result AStarSolver::solve(const GameState& start, int maxIterations, int stepLimit) {
    Result result;
    result.found = false;
    result.moveCount = 0;
    result.moves = nullptr;
    result.expanded = 0;
    result.generated = 0;

    MinHeap open;
    BestCostTable bestCosts;
    NodeArena arena;
    int temporalPeriod = computeTemporalPeriod(start.getBoard());

    GameState::Move dummyMove = {-1, 0, 0};
    AStarNode* root = arena.create(start, nullptr, dummyMove, 0, heuristic(start));
    bestCosts.setIfBetter(makeTemporalKey(start, temporalPeriod), 0);
    open.push(root);

    int iterations = 0;
    while (!open.empty() && iterations < maxIterations) {
        AStarNode* current = open.pop();
        ++result.expanded;

        int knownBest = 0;
        unsigned long currentKey = makeTemporalKey(*current->state, temporalPeriod);
        if (bestCosts.tryGet(currentKey, knownBest) && current->g > knownBest) {
            ++iterations;
            continue;
        }

        if (current->state->isGoal()) {
            int count = 0;
            for (AStarNode* node = current; node != nullptr && node->parent != nullptr; node = node->parent) {
                ++count;
            }

            GameState::Move* moves = new GameState::Move[count];
            int writeIndex = count - 1;
            for (AStarNode* node = current; node != nullptr && node->parent != nullptr; node = node->parent) {
                moves[writeIndex--] = node->moveFromParent;
            }

            result.found = true;
            result.moveCount = count;
            result.moves = moves;
            return result;
        }

        int blocks = current->state->getBlockCount();
        for (int i = 0; i < blocks; ++i) {
            const Block& block = current->state->getBlock(i);
            if (current->state->hasBlockExited(block.getId())) continue;

            GameState::Move movesBuffer[128];
            int moveCount = current->state->generateMovementsForBlock(block.getId(), movesBuffer, 128);

            for (int m = 0; m < moveCount; ++m) {
                GameState nextState = *current->state;
                if (!nextState.applyMove(movesBuffer[m])) continue;
            // prune states that exceed the level's step limit
            if (stepLimit > 0 && nextState.getStep() > stepLimit) continue;

                int moveCost = (movesBuffer[m].distance == 0) ? 0 : movesBuffer[m].distance;
                int newG = current->g + moveCost;
                unsigned long nextKey = makeTemporalKey(nextState, temporalPeriod);
                if (!bestCosts.setIfBetter(nextKey, newG)) continue;
                int newH = heuristic(nextState);

                AStarNode* child = arena.create(nextState, current, movesBuffer[m], newG, newH);
                open.push(child);
                ++result.generated;
            }
        }

        ++iterations;
    }

    // If not found, try a fallback uniform-cost search (heuristic=0) to ensure completeness
    if (!result.found) {
        // clear structures by recreating them
        MinHeap open2;
        BestCostTable best2;
        NodeArena arena2;

        AStarNode* root2 = arena2.create(start, nullptr, dummyMove, 0, 0);
        open2.push(root2);

        int it2 = 0;
        int max2 = maxIterations * 5;
        while (!open2.empty() && it2 < max2) {
            AStarNode* cur = open2.pop();
            ++result.expanded;

            if (cur->state->isGoal()) {
                int count = 0;
                for (AStarNode* node = cur; node != nullptr && node->parent != nullptr; node = node->parent) ++count;
                GameState::Move* moves = new GameState::Move[count];
                int wi = count - 1;
                for (AStarNode* node = cur; node != nullptr && node->parent != nullptr; node = node->parent) moves[wi--] = node->moveFromParent;
                result.found = true; result.moveCount = count; result.moves = moves; return result;
            }

            unsigned long curKey = makeTemporalKey(*cur->state, temporalPeriod);
            int kb = 0; if (best2.tryGet(curKey, kb) && cur->g > kb) { ++it2; continue; }
            best2.setIfBetter(curKey, cur->g);

            int blks = cur->state->getBlockCount();
            for (int i = 0; i < blks; ++i) {
                const Block& block = cur->state->getBlock(i);
                if (cur->state->hasBlockExited(block.getId())) continue;

                GameState::Move movesBuf[128];
                int mcnt = cur->state->generateMovementsForBlock(block.getId(), movesBuf, 128);
                for (int m = 0; m < mcnt; ++m) {
                    GameState ns = *cur->state;
                    if (!ns.applyMove(movesBuf[m])) continue;
                    if (stepLimit > 0 && ns.getStep() > stepLimit) continue;
                    int cost = (movesBuf[m].distance == 0) ? 0 : movesBuf[m].distance;
                    int newG = cur->g + cost;
                    unsigned long nk = makeTemporalKey(ns, temporalPeriod);
                    if (!best2.setIfBetter(nk, newG)) continue;
                    AStarNode* child = arena2.create(ns, cur, movesBuf[m], newG, 0);
                    open2.push(child);
                    ++result.generated;
                }
            }
            ++it2;
        }
    }

    return result;
}

void AStarSolver::freeResult(Result& result) {
    if (result.moves != nullptr) {
        delete[] result.moves;
    }

    result.moves = nullptr;
    result.moveCount = 0;
    result.found = false;
    result.expanded = 0;
    result.generated = 0;
}
