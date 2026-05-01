#include "Board.h"

#include <iostream>

int Board::index(int x, int y) const {
    return (y * width) + x;
}

Board::Board(int _width, int _height)
    : width(_width),
      height(_height),
      wallCount(0),
      exitCount(0),
      gateCount(0),
      maxWalls(100),
      maxExits(10),
      maxGates(10) {
    cells = new int[_width * _height];
    for (int i = 0; i < (_width * _height); ++i) {
        cells[i] = -1;
    }
    walls = new Wall[maxWalls];
    exits = new Exit[maxExits];
    gates = new Gate[maxGates];
}

Board::Board(const Board& other)
    : width(other.width),
      height(other.height),
      wallCount(other.wallCount),
      exitCount(other.exitCount),
      gateCount(other.gateCount),
      maxWalls(other.maxWalls),
      maxExits(other.maxExits),
      maxGates(other.maxGates) {
    cells = new int[width * height];
    for (int i = 0; i < (width * height); ++i) {
        cells[i] = other.cells[i];
    }

    walls = new Wall[maxWalls];
    for (int i = 0; i < wallCount; ++i) {
        walls[i] = other.walls[i];
    }

    exits = new Exit[maxExits];
    for (int i = 0; i < exitCount; ++i) {
        exits[i] = other.exits[i];
    }

    gates = new Gate[maxGates];
    for (int i = 0; i < gateCount; ++i) {
        gates[i] = other.gates[i];
    }
}

Board& Board::operator=(const Board& other) {
    if (this == &other) {
        return *this;
    }

    delete[] cells;
    delete[] walls;
    delete[] exits;
    delete[] gates;

    width = other.width;
    height = other.height;
    wallCount = other.wallCount;
    exitCount = other.exitCount;
    gateCount = other.gateCount;
    maxWalls = other.maxWalls;
    maxExits = other.maxExits;
    maxGates = other.maxGates;

    cells = new int[width * height];
    for (int i = 0; i < (width * height); ++i) {
        cells[i] = other.cells[i];
    }

    walls = new Wall[maxWalls];
    for (int i = 0; i < wallCount; ++i) {
        walls[i] = other.walls[i];
    }

    exits = new Exit[maxExits];
    for (int i = 0; i < exitCount; ++i) {
        exits[i] = other.exits[i];
    }

    gates = new Gate[maxGates];
    for (int i = 0; i < gateCount; ++i) {
        gates[i] = other.gates[i];
    }

    return *this;
}

Board::~Board() {
    delete[] cells;
    delete[] walls;
    delete[] exits;
    delete[] gates;
}

int Board::getWidth() const {
    return width;
}

int Board::getHeight() const {
    return height;
}

bool Board::isInside(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::isCellFree(int x, int y) const {
    if (!isInside(x, y)) {
        return false;
    }
    return cells[index(x, y)] == -1;
}

bool Board::canPlaceBlock(const Block& block) const {
    const int blockX = block.getX();
    const int blockY = block.getY();
    const int blockWidth = block.getWidth();
    const int blockHeight = block.getHeight();
    const bool* geometry = block.getGeometry();

    for (int localY = 0; localY < blockHeight; ++localY) {
        for (int localX = 0; localX < blockWidth; ++localX) {
            const bool isFilled = (geometry == nullptr)
                ? true
                : geometry[(localY * blockWidth) + localX];

            if (!isFilled) {
                continue;
            }

            const int worldX = blockX + localX;
            const int worldY = blockY + localY;

            if (!isInside(worldX, worldY) || !isCellFree(worldX, worldY) || isWallAt(worldX, worldY)) {
                return false;
            }
        }
    }

    return true;
}

bool Board::placeBlock(const Block& block) {
    if (!canPlaceBlock(block)) {
        return false;
    }

    const int blockX = block.getX();
    const int blockY = block.getY();
    const int blockWidth = block.getWidth();
    const int blockHeight = block.getHeight();
    const bool* geometry = block.getGeometry();

    for (int localY = 0; localY < blockHeight; ++localY) {
        for (int localX = 0; localX < blockWidth; ++localX) {
            const bool isFilled = (geometry == nullptr)
                ? true
                : geometry[(localY * blockWidth) + localX];

            if (!isFilled) {
                continue;
            }

            const int worldX = blockX + localX;
            const int worldY = blockY + localY;
            cells[index(worldX, worldY)] = block.getId();
        }
    }

    return true;
}

void Board::removeBlock(const Block& block) {
    const int blockX = block.getX();
    const int blockY = block.getY();
    const int blockWidth = block.getWidth();
    const int blockHeight = block.getHeight();
    const bool* geometry = block.getGeometry();

    for (int localY = 0; localY < blockHeight; ++localY) {
        for (int localX = 0; localX < blockWidth; ++localX) {
            const bool isFilled = (geometry == nullptr)
                ? true
                : geometry[(localY * blockWidth) + localX];

            if (!isFilled) {
                continue;
            }

            const int worldX = blockX + localX;
            const int worldY = blockY + localY;

            if (isInside(worldX, worldY)) {
                cells[index(worldX, worldY)] = -1;
            }
        }
    }
}

void Board::clear() {
    for (int i = 0; i < (width * height); ++i) {
        cells[i] = -1;
    }
}

bool Board::addWall(const Wall& wall) {
    if (wallCount >= maxWalls) {
        return false;
    }
    walls[wallCount] = wall;
    ++wallCount;
    return true;
}

bool Board::addExit(const Exit& exit) {
    if (exitCount >= maxExits) {
        return false;
    }
    exits[exitCount] = exit;
    ++exitCount;
    return true;
}

bool Board::addGate(const Gate& gate) {
    if (gateCount >= maxGates) {
        return false;
    }
    gates[gateCount] = gate;
    ++gateCount;
    return true;
}

bool Board::isWallAt(int x, int y) const {
    for (int i = 0; i < wallCount; ++i) {
        if (walls[i].occupiesCell(x, y)) {
            return true;
        }
    }
    return false;
}

int Board::getWallIndex(int x, int y) const {
    for (int i = 0; i < wallCount; ++i) {
        if (walls[i].occupiesCell(x, y)) {
            return i;
        }
    }
    return -1;
}

int Board::getWallCount() const {
    return wallCount;
}

int Board::getExitCount() const {
    return exitCount;
}

int Board::getGateCount() const {
    return gateCount;
}

void Board::print() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isWallAt(x, y)) {
                std::cout << "# ";
            } else {
                const int value = cells[index(x, y)];
                if (value < 0) {
                    std::cout << ". ";
                } else {
                    std::cout << value << " ";
                }
            }
        }
        std::cout << '\n';
    }
}
