#pragma once

#include "Block.h"

class Board {
private:
    int width;
    int height;
    int* cells;

    // Convierte coordenadas 2D a indice lineal interno.
    int index(int x, int y) const;

public:
    // Crea un tablero vacio de ancho x alto.
    Board(int _width, int _height);

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

    // Limpia completamente el tablero.
    void clear();

    // Imprime una vista simple para depuracion.
    void print() const;

    // Destructor para liberar memoria del arreglo dinamico.
    ~Board();
};
