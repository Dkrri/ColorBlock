#pragma once

class Block {
private:
    int id;
    int colorLock;
    char color;
    int ancho;
    int altura;
    int x;
    int y;
    bool* geometry;

public:
    // Constructor por defecto.
    Block();

    // Constructor base para inicializar un bloque del tablero.
    Block(int _id, int _colorLock, char _color, int _ancho, int _altura, int _x, int _y, bool* _geo);

    // Getters basicos de informacion del bloque.
    int getId() const;
    int getColorLock() const;
    char getColor() const;
    int getWidth() const;
    int getHeight() const;
    int getX() const;
    int getY() const;
    bool* getGeometry() const;

    // Permite actualizar el color del bloque.
    void setColor(char newColor);

    // Permite disminuir el bloqueo de color si sale algun bloque
    void decrementColorLock();

    // Reposiciona el bloque directamente en el tablero.
    void setPosition(int newX, int newY);
    void setX(int newX);
    void setY(int newY);

    // Mueve el bloque con un desplazamiento relativo.
    void moveBy(int dx, int dy);

    // Verifica si una celda esta dentro del area rectangular ocupada.
    bool occupiesCell(int cellX, int cellY) const;

    // Verifica si dos bloques se intersectan considerando su geometria real.
    bool collidesWith(const Block& other) const;
};
