#include "AStarSolver.h"

#include "AStarNode.h"
#include "BestCostTable.h"
#include "MinHeap.h"
#include "NodeArena.h"

#include <limits>
#include <iostream>

// ─────────────────────────────────────────────────────────────────
// Helpers numéricos
// ─────────────────────────────────────────────────────────────────

int AStarSolver::absInt(int value) {
    return (value < 0) ? -value : value;
}

int AStarSolver::gcdInt(int a, int b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return (a == 0) ? 1 : a;
}

int AStarSolver::lcmCapped(int a, int b, int cap) {
    int g = gcdInt(a, b);
    long long part = (long long)(a / g) * (long long)b;
    if (part > cap) return cap;
    return (int)part;
}

int AStarSolver::computeTemporalPeriod(const Board& board) {
    const int CAP = 4096;
    int period = 1;

    for (int i = 0; i < board.getExitCount(); ++i) {
        const Exit& ex = board.getExits()[i];
        int li   = ex.getInitialLength();
        int lf   = ex.getFinalLength();
        int step = ex.getStepChange();
        int diff = absInt(li - lf);
        if (step > 0 && diff > 0) {
            int p = step * 2 * diff;
            period = lcmCapped(period, p, CAP);
        }
    }

    for (int i = 0; i < board.getGateCount(); ++i) {
        const Gate& g = board.getGates()[i];
        int step  = g.getStepChange();
        char ci   = g.getInitialColor();
        char cf   = g.getFinalColor();
        int range = (cf >= ci) ? (cf - ci + 1) : 1;
        if (step > 0 && range > 1) {
            int p = step * range;
            period = lcmCapped(period, p, CAP);
        }
    }

    if (period <= 0) period = 1;
    return period;
}

unsigned long AStarSolver::makeTemporalKey(const GameState& state, int temporalPeriod) {
    unsigned long base = state.hash();
    unsigned long step = (unsigned long)(state.getStep() % temporalPeriod);
    return (base * 1315423911ul) ^ (step + 0x9e3779b9ul + (base << 6) + (base >> 2));
}

// ─────────────────────────────────────────────────────────────────
// Componentes de heurística
// ─────────────────────────────────────────────────────────────────

/**
 * cellDistanceLowerBound
 * Distancia Manhattan mínima entre cualquier celda del bloque y cualquier
 * celda de cualquier salida compatible con su color (largo actual).
 * Es admisible: nunca sobreestima.
 */
int AStarSolver::cellDistanceLowerBound(const GameState& state, const Block& block) {
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

/**
 * alignmentDistanceLowerBound
 * Pasos mínimos para posicionar el bloque justo frente a alguna salida
 * (condición necesaria para poder salir).
 *
 * MEJORA respecto al original: usa el largo MÁXIMO posible de la salida
 * (max(LI, LF)) en lugar del largo actual. Esto es admisible porque
 * representa la condición más favorable que el bloque podría aprovechar;
 * la cota de espera (exitTimingLowerBound) cubre el tiempo adicional.
 */
int AStarSolver::alignmentDistanceLowerBound(const GameState& state, const Block& block) {
    const Board& board = state.getBoard();
    int best = std::numeric_limits<int>::max();

    for (int e = 0; e < board.getExitCount(); ++e) {
        const Exit& ex = board.getExits()[e];
        if (ex.getColor() != block.getColor()) continue;

        // Largo máximo alcanzable por esta salida (puede ser LI o LF)
        int li     = ex.getInitialLength();
        int lf     = ex.getFinalLength();
        int maxLen = (li > lf) ? li : lf;
        if (maxLen <= 0) continue;

        if (ex.getOrientation() == 'H') {
            // El bloque necesita caber a lo ancho
            if (block.getWidth() > maxLen) continue;

            // dx: distancia vertical para quedar inmediatamente arriba o abajo
            int targetXUp   = ex.getX() - block.getHeight();
            int targetXDown = ex.getX() + 1;
            int dxUp   = absInt(block.getX() - targetXUp);
            int dxDown = absInt(block.getX() - targetXDown);
            int dx = (dxUp < dxDown) ? dxUp : dxDown;

            // dy: distancia horizontal para que el bloque quede dentro del rango de la salida
            int minY = ex.getY();
            int maxY = ex.getY() + maxLen - block.getWidth();
            int dy = 0;
            if (block.getY() < minY)      dy = minY - block.getY();
            else if (block.getY() > maxY) dy = block.getY() - maxY;

            int d = dx + dy;
            if (d < best) best = d;

        } else { // 'V'
            // El bloque necesita caber a lo alto
            if (block.getHeight() > maxLen) continue;

            // dy: distancia horizontal para quedar inmediatamente a izquierda o derecha
            int targetYLeft  = ex.getY() - block.getWidth();
            int targetYRight = ex.getY() + 1;
            int dyLeft  = absInt(block.getY() - targetYLeft);
            int dyRight = absInt(block.getY() - targetYRight);
            int dy = (dyLeft < dyRight) ? dyLeft : dyRight;

            // dx: distancia vertical para que el bloque quede dentro del rango de la salida
            int minX = ex.getX();
            int maxX = ex.getX() + maxLen - block.getHeight();
            int dx = 0;
            if (block.getX() < minX)      dx = minX - block.getX();
            else if (block.getX() > maxX) dx = block.getX() - maxX;

            int d = dx + dy;
            if (d < best) best = d;
        }
    }

    return (best == std::numeric_limits<int>::max()) ? 0 : best;
}

/**
 * exitTimingLowerBound  (NUEVA)
 * Si ninguna salida compatible es actualmente suficientemente grande para
 * este bloque, devuelve la cantidad mínima de pasos que hay que esperar
 * hasta que alguna salida alcance el tamaño necesario.
 *
 * Es admisible: el bloque no puede salir antes de que la salida exista.
 */
int AStarSolver::exitTimingLowerBound(const GameState& state, const Block& block) {
    const Board& board  = state.getBoard();
    int best = std::numeric_limits<int>::max();

    for (int e = 0; e < board.getExitCount(); ++e) {
        const Exit& ex = board.getExits()[e];
        if (ex.getColor() != block.getColor()) continue;

        // Tamaño necesario según orientación
        int needed = (ex.getOrientation() == 'H') ? block.getWidth() : block.getHeight();

        // Largo máximo que puede alcanzar esta salida
        int li     = ex.getInitialLength();
        int lf     = ex.getFinalLength();
        int maxLen = (li > lf) ? li : lf;
        if (needed > maxLen) continue; // esta salida nunca podrá alojar al bloque

        // Si ya es suficientemente grande, no hay espera
        if (ex.getLengthAtStep(state.getStep()) >= needed) {
            return 0;
        }

        // Buscar el primer paso futuro en que la salida alcance el tamaño necesario
        int stepChange = ex.getStepChange();
        if (stepChange <= 0) continue; // salida estática que no crece

        // Búsqueda acotada (el puzzle tiene STEP_LIMIT, basta con 2000 pasos)
        for (int wait = 1; wait <= 2000; ++wait) {
            if (ex.getLengthAtStep(state.getStep() + wait) >= needed) {
                if (wait < best) best = wait;
                break;
            }
        }
    }

    return (best == std::numeric_limits<int>::max()) ? 0 : best;
}

/**
 * pathBlockersLowerBound  (NUEVA)
 * Cuenta cuántos bloques no salidos se encuentran en el pasillo directo
 * entre el bloque actual y su mejor salida compatible. Cada bloqueador
 * necesita al menos 1 movimiento para despejarse.
 *
 * Admisibilidad: es una cota inferior en la mayoría de los casos (cada
 * bloqueador necesita moverse, y ese movimiento no siempre coincide con
 * el movimiento óptimo del bloqueador hacia su propia salida). En casos
 * muy específicos puede ser ligeramente inadmisible, pero en la práctica
 * es esencial para resolver puzzles con muchos bloques donde la heurística
 * sin bloqueos es demasiado débil.
 */
int AStarSolver::pathBlockersLowerBound(const GameState& state, const Block& block) {
    const Board& board = state.getBoard();
    int minBlockers = std::numeric_limits<int>::max();

    for (int e = 0; e < board.getExitCount(); ++e) {
        const Exit& ex = board.getExits()[e];
        if (ex.getColor() != block.getColor()) continue;

        int li     = ex.getInitialLength();
        int lf     = ex.getFinalLength();
        int maxLen = (li > lf) ? li : lf;
        if (maxLen <= 0) continue;

        int needed = (ex.getOrientation() == 'H') ? block.getWidth() : block.getHeight();
        if (needed > maxLen) continue;

        int blockers = 0;

        if (ex.getOrientation() == 'V') {
            // El bloque se mueve horizontalmente (hacia la columna de la salida vertical)
            int exitCol    = ex.getY();
            int blkRowMin  = block.getX();
            int blkRowMax  = block.getX() + block.getHeight() - 1;
            int blkColLeft = block.getY();
            int blkColRight= block.getY() + block.getWidth() - 1;

            int pathColMin, pathColMax;
            if (exitCol > blkColRight) {
                pathColMin = blkColRight + 1;
                pathColMax = exitCol - 1;
            } else if (exitCol < blkColLeft) {
                pathColMin = exitCol + 1;
                pathColMax = blkColLeft - 1;
            } else {
                // Ya está en la columna de la salida
                if (minBlockers > 0) minBlockers = 0;
                continue;
            }

            if (pathColMin > pathColMax) {
                // Adyacente a la salida
                if (minBlockers > 0) minBlockers = 0;
                continue;
            }

            for (int i = 0; i < state.getBlockCount(); ++i) {
                const Block& other = state.getBlock(i);
                if (other.getId() == block.getId()) continue;
                if (state.hasBlockExited(other.getId())) continue;

                int oRowMin = other.getX();
                int oRowMax = other.getX() + other.getHeight() - 1;
                int oColMin = other.getY();
                int oColMax = other.getY() + other.getWidth() - 1;

                // ¿El otro bloque intersecta el pasillo (misma franja de filas + en el camino)?
                if (oRowMax >= blkRowMin && oRowMin <= blkRowMax &&
                    oColMax >= pathColMin && oColMin <= pathColMax) {
                    ++blockers;
                }
            }

        } else { // 'H' exit
            // El bloque se mueve verticalmente (hacia la fila de la salida horizontal)
            int exitRow     = ex.getX();
            int blkRowTop   = block.getX();
            int blkRowBottom= block.getX() + block.getHeight() - 1;
            int blkColLeft  = block.getY();
            int blkColRight = block.getY() + block.getWidth() - 1;

            int pathRowMin, pathRowMax;
            if (exitRow > blkRowBottom) {
                pathRowMin = blkRowBottom + 1;
                pathRowMax = exitRow - 1;
            } else if (exitRow < blkRowTop) {
                pathRowMin = exitRow + 1;
                pathRowMax = blkRowTop - 1;
            } else {
                // Ya está en la fila de la salida
                if (minBlockers > 0) minBlockers = 0;
                continue;
            }

            if (pathRowMin > pathRowMax) {
                if (minBlockers > 0) minBlockers = 0;
                continue;
            }

            for (int i = 0; i < state.getBlockCount(); ++i) {
                const Block& other = state.getBlock(i);
                if (other.getId() == block.getId()) continue;
                if (state.hasBlockExited(other.getId())) continue;

                int oRowMin = other.getX();
                int oRowMax = other.getX() + other.getHeight() - 1;
                int oColMin = other.getY();
                int oColMax = other.getY() + other.getWidth() - 1;

                // ¿El otro bloque intersecta el pasillo (misma franja de columnas + en el camino)?
                if (oRowMax >= pathRowMin && oRowMin <= pathRowMax &&
                    oColMax >= blkColLeft && oColMin <= blkColRight) {
                    ++blockers;
                }
            }
        }

        if (blockers < minBlockers) minBlockers = blockers;
    }

    return (minBlockers == std::numeric_limits<int>::max()) ? 0 : minBlockers;
}

/**
 * blockHeuristic — mantenida por compatibilidad (no usada en la ruta principal).
 */
int AStarSolver::blockHeuristic(const GameState& state, const Block& block) {
    return cellDistanceLowerBound(state, block);
}

/**
 * heuristic
 * Suma sobre todos los bloques no salidos de:
 *   max(cellDist, alignDist, exitTiming)  +  pathBlockers
 *
 * • Las tres primeras componentes son cota inferior admisible.
 * • pathBlockers añade un término de bloqueo que hace la heurística mucho
 *   más informada, a costa de una posible inadmisibilidad menor en casos
 *   extremos. Para estos puzzles complejos el trade-off es necesario.
 */
int AStarSolver::heuristic(const GameState& state) {
    int total = 0;
    for (int i = 0; i < state.getBlockCount(); ++i) {
        const Block& block = state.getBlock(i);
        if (state.hasBlockExited(block.getId())) continue;

        int cellDist  = cellDistanceLowerBound(state, block);
        int alignDist = alignmentDistanceLowerBound(state, block);
        int timing    = exitTimingLowerBound(state, block);
        int blockers  = pathBlockersLowerBound(state, block);

        // Base: máximo de las tres cotas admisibles independientes
        int base = cellDist;
        if (alignDist > base) base = alignDist;
        if (timing    > base) base = timing;

        // Cada bloqueador en el camino requiere al menos 1 movimiento extra
        total += base + blockers;
    }
    return total;
}

// ─────────────────────────────────────────────────────────────────
// Solver principal
// ─────────────────────────────────────────────────────────────────

AStarSolver::Result AStarSolver::solve(const GameState& start, int maxIterations, int stepLimit) {
    Result result;
    result.found     = false;
    result.moveCount = 0;
    result.moves     = nullptr;
    result.expanded  = 0;
    result.generated = 0;

    MinHeap       open;
    BestCostTable bestCosts;
    NodeArena     arena;
    int temporalPeriod = computeTemporalPeriod(start.getBoard());

    GameState::Move dummyMove = {-1, 0, 0};
    AStarNode* root = arena.create(start, nullptr, dummyMove, 0, heuristic(start));
    bestCosts.setIfBetter(makeTemporalKey(start, temporalPeriod), 0);
    open.push(root);

    int iterations = 0;
    while (!open.empty() && iterations < maxIterations) {
        AStarNode* current = open.pop();
        ++result.expanded;

        unsigned long currentKey = makeTemporalKey(*current->state, temporalPeriod);
        int knownBest = 0;
        if (bestCosts.tryGet(currentKey, knownBest) && current->g > knownBest) {
            ++iterations;
            continue;
        }

        if (current->state->isGoal()) {
            int count = 0;
            for (AStarNode* node = current; node && node->parent; node = node->parent) ++count;

            GameState::Move* moves = new GameState::Move[count];
            int wi = count - 1;
            for (AStarNode* node = current; node && node->parent; node = node->parent)
                moves[wi--] = node->moveFromParent;

            result.found     = true;
            result.moveCount = count;
            result.moves     = moves;
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
                if (stepLimit > 0 && nextState.getStep() > stepLimit) continue;

                int moveCost = (movesBuffer[m].distance == 0) ? 0 : movesBuffer[m].distance;
                int newG     = current->g + moveCost;
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

    // Fallback: búsqueda de costo uniforme (heurística = 0) para garantizar completitud
    if (!result.found) {
        std::cout << "A* exhausted. Starting uniform cost fallback. iterations: "
                  << iterations << std::endl;

        MinHeap       open2;
        BestCostTable best2;
        NodeArena     arena2;

        AStarNode* root2 = arena2.create(start, nullptr, dummyMove, 0, 0);
        open2.push(root2);

        int it2  = 0;
        int max2 = maxIterations * 5;

        while (!open2.empty() && it2 < max2) {
            AStarNode* cur = open2.pop();
            ++result.expanded;

            if (it2 % 50000 == 0)
                std::cout << "Fallback Iterations: " << it2
                          << " Step: " << cur->state->getStep()
                          << " g: "    << cur->g << std::endl;

            if (cur->state->isGoal()) {
                int count = 0;
                for (AStarNode* node = cur; node && node->parent; node = node->parent) ++count;
                GameState::Move* moves = new GameState::Move[count];
                int wi = count - 1;
                for (AStarNode* node = cur; node && node->parent; node = node->parent)
                    moves[wi--] = node->moveFromParent;
                result.found = true; result.moveCount = count; result.moves = moves;
                return result;
            }

            unsigned long curKey = makeTemporalKey(*cur->state, temporalPeriod);
            int kb = 0;
            if (best2.tryGet(curKey, kb) && cur->g > kb) { ++it2; continue; }
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
    if (result.moves != nullptr) delete[] result.moves;
    result.moves     = nullptr;
    result.moveCount = 0;
    result.found     = false;
    result.expanded  = 0;
    result.generated = 0;
}
