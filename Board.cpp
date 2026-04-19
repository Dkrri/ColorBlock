#include "Board.h"

#include <iostream>

int Board::index(int x, int y) const {
    return (y * width) + x;
}

Board::Board(int _width, int _height)
    : width(_width),
      height(_height) {
    cells = new int[_width * _height];
    for (int i = 0; i < (_width * _height); ++i) {
        cells[i] = -1;
    }
}

Board::~Board() {
    delete[] cells;
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

            if (!isInside(worldX, worldY) || !isCellFree(worldX, worldY)) {
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

void Board::print() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int value = cells[index(x, y)];
            if (value < 0) {
                std::cout << ". ";
            } else {
                std::cout << value << " ";
            }
        }
        std::cout << '\n';
    }
}
