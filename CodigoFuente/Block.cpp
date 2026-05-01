#include "Block.h"

Block::Block()
    : id(0),
      colorLock(0),
      color(' '),
      ancho(0),
      altura(0),
      x(0),
      y(0),
      geometry(nullptr) {}

// Inicializa todos los atributos del bloque al crearlo.
Block::Block(int _id, int _colorLock, char _color, int _ancho, int _altura, int _x, int _y, bool* _geo)
    : id(_id),
      colorLock(_colorLock),
      color(_color),
      ancho(_ancho),
      altura(_altura),
      x(_x),
      y(_y),
      geometry(_geo) {}

int Block::getId() const {
    return id;
}

int Block::getColorLock() const {
    return colorLock;
}

char Block::getColor() const {
    return color;
}

int Block::getWidth() const {
    return ancho;
}

int Block::getHeight() const {
    return altura;
}

int Block::getX() const {
    return x;
}

int Block::getY() const {
    return y;
}

bool* Block::getGeometry() const {
    return geometry;
}

void Block::setColor(char newColor) {
    color = newColor;
}

void Block::decrementColorLock() {
    if (colorLock > 0) {
        colorLock--;
    }
}

void Block::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Block::moveBy(int dx, int dy) {
    x += dx;
    y += dy;
}

bool Block::occupiesCell(int cellX, int cellY) const {
    return cellX >= x && cellX < (x + ancho) &&
           cellY >= y && cellY < (y + altura);
}
