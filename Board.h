#pragma once

#include "Block.h"
#include "Wall.h"
#include "Exit.h"
#include "Gate.h"

class Board {
private:
    int width;
    int height;
    int* cells;
    Wall* walls;
    Exit* exits;
    Gate* gates;
    int wallCount;
    int exitCount;
    int gateCount;
    int maxWalls;
    int maxExits;
    int maxGates;

    // Convierte coordenadas 2D a indice lineal interno.
    int index(int x, int y) const;

public:
    // Crea un tablero vacio de ancho x alto.
    Board(int _width, int _height);

    // Constructor copia con copia profunda de arreglos internos.
    Board(const Board& other);

    // Operador asignacion con copia profunda.
    Board& operator=(const Board& other);

    // Getters basicos del tamano del tablero.
    int getWidth() const;
    int getHeight() const;

    // Valida si una coordenada pertenece al tablero.
    bool isInside(int x, int y) const;

    // Consulta si la celda esta libre.
    bool isCellFree(int x, int y) const;

    // Verifica si un bloque puede ubicarse sin salir ni colisionar.
    bool canPlaceBlock(const Block& block) const;

    // Intenta ubicar un bloque. Devuelve false si no se puede.
    bool placeBlock(const Block& block);

    // Elimina del tablero todas las celdas ocupadas por ese bloque.
    void removeBlock(const Block& block);

    // Agrega una pared al tablero. Devuelve false si lleno.
    bool addWall(const Wall& wall);

    // Agrega una salida al tablero. Devuelve false si lleno.
    bool addExit(const Exit& exit);

    // Agrega una compuerta al tablero. Devuelve false si lleno.
    bool addGate(const Gate& gate);

    // Verifica si hay pared en una coordenada.
    bool isWallAt(int x, int y) const;

    // Obtiene el indice de pared en coordenada, o -1 si no existe.
    int getWallIndex(int x, int y) const;

    // Obtiene numero de paredes agregadas.
    int getWallCount() const;

    // Obtiene numero de salidas agregadas.
    int getExitCount() const;

    // Obtiene numero de compuertas agregadas.
    int getGateCount() const;

    const Exit* getExits() const { return exits; }
    const Gate* getGates() const { return gates; }

    // Limpia completamente el tablero.
    void clear();

    // Imprime una vista simple para depuracion.
    void print() const;

    // Destructor para liberar memoria de arreglos dinamicos.
    ~Board();
};
