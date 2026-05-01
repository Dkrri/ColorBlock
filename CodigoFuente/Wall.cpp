#include "Wall.h"

Wall::Wall()
    : x(0),
      y(0),
      exitWall(false),
      gateWall(false) {}

Wall::Wall(int _x, int _y, bool _isExit, bool _isGate)
    : x(_x),
      y(_y),
      exitWall(_isExit),
      gateWall(_isGate) {}

int Wall::getX() const {
    return x;
}

int Wall::getY() const {
    return y;
}

bool Wall::isExitWall() const {
    return exitWall;
}

bool Wall::isGateWall() const {
    return gateWall;
}

void Wall::setExitWall(bool value) {
    exitWall = value;
}

void Wall::setGateWall(bool value) {
    gateWall = value;
}

void Wall::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

bool Wall::occupiesCell(int cellX, int cellY) const {
    return x == cellX && y == cellY;
}
