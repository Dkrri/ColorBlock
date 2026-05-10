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

    // --- Componentes de la heurística ---

    // Cota inferior: distancia Manhattan de celdas del bloque a la salida más cercana.
    static int cellDistanceLowerBound(const GameState& state, const Block& block);

    // Cota inferior: pasos mínimos para alinear el bloque frente a alguna salida.
    // Usa el largo MÁXIMO posible de la salida (admisible aunque la salida aún no crezca).
    static int alignmentDistanceLowerBound(const GameState& state, const Block& block);

    // Cota inferior: pasos mínimos de espera hasta que alguna salida compatible sea
    // suficientemente grande para este bloque.
    static int exitTimingLowerBound(const GameState& state, const Block& block);

    // Cota inferior de bloqueo: cantidad mínima de bloques que obstruyen el camino
    // directo del bloque hacia su mejor salida. Cada bloqueador requiere ≥1 movimiento
    // adicional. Ligeramente inadmisible en casos degenerados, pero muy efectiva en
    // puzzles complejos donde la admisibilidad estricta impide resolver el nivel.
    static int pathBlockersLowerBound(const GameState& state, const Block& block);

    // Heurística agregada (suma sobre todos los bloques no salidos).
    static int heuristic(const GameState& state);

    // Kept for backwards-compat (unused in main heuristic path).
    static int blockHeuristic(const GameState& state, const Block& block);
};
