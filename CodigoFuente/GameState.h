#pragma once

#include "Block.h"
#include "Board.h"

class GameState {
public:
    // Constantes para direcciones de movimiento.
    static const int DIR_UP = 0;
    static const int DIR_DOWN = 1;
    static const int DIR_LEFT = 2;
    static const int DIR_RIGHT = 3;
    static const int DIR_COUNT = 4;

    // Estructura para almacenar un movimiento.
    struct Move {
        int blockId;
        int direction;
        int distance;
    };

private:
    Board board;
    Block* blocks;
    int blockCount;
    int currentStep;
    bool* blocksExited;
    int maxBlocks;

public:
    // Constructor que crea un estado inicial basado en un tablero.
    GameState(const Board& initialBoard, Block* initialBlocks, int blockCount);

    // Constructor copia para duplicar estado.
    GameState(const GameState& other);

    // Operador asignacion.
    GameState& operator=(const GameState& other);

    // Destructor.
    ~GameState();

    // Getters basicos.
    int getStep() const;
    int getBlockCount() const;
    const Board& getBoard() const;
    const Block& getBlock(int index) const;
    bool hasBlockExited(int blockId) const;

    // Verifica si es el estado objetivo (todos bloques salieron).
    bool isGoal() const;

    // Intenta generar movimientos validos para un bloque.
    // Retorna cantidad de movimientos generados.
    int generateMovementsForBlock(int blockId, Move* movements, int maxMovements) const;

    // Aplica un movimiento al estado. Modifica el estado actual.
    bool applyMove(const Move& move);

    // Desaplica un movimiento (revierte).
    bool undoMove(const Move& move);

    // Calcula un hash para identificar el estado (usado en A*).
    unsigned long hash() const;

    // Compara dos estados.
    bool equals(const GameState& other) const;

    // Avanza el paso de simulacion.
    void advanceStep();

    // Imprime el estado actual para depuracion.
    void print() const;

private:
    // Metodos auxiliares fisicos para evaluacion sin STL.
    int findBlockIndexById(int blockId) const;
    bool isValidPlacement(const Block& block, int ignoreBlockId) const;
    bool canExitThroughAny(const Block& block) const;
    bool canExitThroughExit(const Block& block, const Exit& ex) const;
    bool tryGateCross(const Block& block, int direction, Block& outBlock) const;
};
