#include "GameState.h"
#include <iostream>

GameState::GameState(const Board& initialBoard, Block* initialBlocks, int initialBlockCount)
    : board(initialBoard),
      blockCount(initialBlockCount),
      currentStep(0),
      maxBlocks(initialBlockCount) {
    blocks = new Block[maxBlocks];
    blocksExited = new bool[maxBlocks];

    for (int i = 0; i < blockCount; ++i) {
        blocks[i] = initialBlocks[i];
        blocksExited[i] = false;
    }
}

GameState::GameState(const GameState& other)
    : board(other.board),
      blockCount(other.blockCount),
      currentStep(other.currentStep),
      maxBlocks(other.maxBlocks) {
    blocks = new Block[maxBlocks];
    blocksExited = new bool[maxBlocks];

    for (int i = 0; i < blockCount; ++i) {
        blocks[i] = other.blocks[i];
        blocksExited[i] = other.blocksExited[i];
    }
}

GameState& GameState::operator=(const GameState& other) {
    if (this == &other) {
        return *this;
    }

    delete[] blocks;
    delete[] blocksExited;

    board = other.board;
    blockCount = other.blockCount;
    currentStep = other.currentStep;
    maxBlocks = other.maxBlocks;

    blocks = new Block[maxBlocks];
    blocksExited = new bool[maxBlocks];
    for (int i = 0; i < blockCount; ++i) {
        blocks[i] = other.blocks[i];
        blocksExited[i] = other.blocksExited[i];
    }

    return *this;
}

GameState::~GameState() {
    delete[] blocks;
    delete[] blocksExited;
}

int GameState::findBlockIndexById(int blockId) const {
    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getId() == blockId) {
            return i;
        }
    }
    return -1;
}

int GameState::generateMovementsForBlock(int blockId, Move* movements, int maxMovements) const {
    if (movements == nullptr || maxMovements <= 0) {
        return 0;
    }

    const int index = findBlockIndexById(blockId);
    if (index < 0 || blocksExited[index]) {
        return 0;
    }

    int count = 0;
    const int maxDistance = board.getWidth() + board.getHeight();

    for (int direction = 0; direction < DIR_COUNT; ++direction) {
        for (int distance = 1; distance <= maxDistance && count < maxMovements; ++distance) {
            Move move;
            move.blockId = blockId;
            move.direction = direction;
            move.distance = distance;

            GameState candidate(*this);
            if (candidate.applyMove(move)) {
                movements[count++] = move;
            } else {
                break;
            }
        }
    }

    return count;
}

bool GameState::applyMove(const Move& move) {
    const int index = findBlockIndexById(move.blockId);
    if (index < 0 || blocksExited[index] || move.distance <= 0) {
        return false;
    }

    Block& block = blocks[index];
    const int oldX = block.getX();
    const int oldY = block.getY();

    Block gateTarget;
    if (move.distance == 1 && tryGateCross(block, move.direction, gateTarget)) {
        block = gateTarget;
        currentStep += 1;
        return true;
    }

    int dx = 0;
    int dy = 0;
    if (move.direction == DIR_UP) {
        dx = -1;
    } else if (move.direction == DIR_DOWN) {
        dx = 1;
    } else if (move.direction == DIR_LEFT) {
        dy = -1;
    } else if (move.direction == DIR_RIGHT) {
        dy = 1;
    } else {
        return false;
    }

    for (int step = 0; step < move.distance; ++step) {
        block.moveBy(dx, dy);
        if (!isValidPlacement(block, block.getId())) {
            block.setPosition(oldX, oldY);
            return false;
        }
    }

    currentStep += move.distance;

    if (canExitThroughAny(block)) {
        blocksExited[index] = true;
        block.setPosition(-100, -100);

        for (int i = 0; i < blockCount; ++i) {
            if (!blocksExited[i]) {
                blocks[i].decrementColorLock();
            }
        }
    }

    return true;
}

bool GameState::undoMove(const Move& move) {
    const int index = findBlockIndexById(move.blockId);
    if (index < 0 || blocksExited[index]) {
        return false;
    }

    Block& block = blocks[index];
    if (move.direction == DIR_UP) {
        block.moveBy(move.distance, 0);
    } else if (move.direction == DIR_DOWN) {
        block.moveBy(-move.distance, 0);
    } else if (move.direction == DIR_LEFT) {
        block.moveBy(0, move.distance);
    } else if (move.direction == DIR_RIGHT) {
        block.moveBy(0, -move.distance);
    }

    currentStep -= move.distance;
    if (currentStep < 0) {
        currentStep = 0;
    }

    return true;
}

bool GameState::isValidPlacement(const Block& block, int ignoreBlockId) const {
    for (int row = block.getX(); row < block.getX() + block.getHeight(); ++row) {
        for (int col = block.getY(); col < block.getY() + block.getWidth(); ++col) {
            if (!block.occupiesCell(row, col)) {
                continue;
            }

            if (!board.isInside(row, col) || board.isWallAt(row, col)) {
                return false;
            }

            for (int g = 0; g < board.getGateCount(); ++g) {
                const Gate& gate = board.getGates()[g];
                if (gate.getX() == row && gate.getY() == col) {
                    return false;
                }
            }
        }
    }

    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getId() == ignoreBlockId || blocksExited[i]) {
            continue;
        }
        if (block.collidesWith(blocks[i])) {
            return false;
        }
    }

    return true;
}

bool GameState::canExitThroughAny(const Block& block) const {
    if (block.getColorLock() > 0) {
        return false;
    }

    for (int e = 0; e < board.getExitCount(); ++e) {
        if (canExitThroughExit(block, board.getExits()[e])) {
            return true;
        }
    }

    return false;
}

bool GameState::canExitThroughExit(const Block& block, const Exit& ex) const {
    if (ex.getColor() != block.getColor()) {
        return false;
    }

    const int len = ex.getLengthAtStep(currentStep);
    if (len <= 0) {
        return false;
    }

    const int blockTop = block.getX();
    const int blockBottom = block.getX() + block.getHeight() - 1;
    const int blockLeft = block.getY();
    const int blockRight = block.getY() + block.getWidth() - 1;

    if (ex.getOrientation() == 'V') {
        const int exitTop = ex.getX();
        const int exitBottom = ex.getX() + len - 1;
        const int exitCol = ex.getY();

        if (block.getHeight() > len) {
            return false;
        }
        if (blockTop < exitTop || blockBottom > exitBottom) {
            return false;
        }

        return (blockRight + 1 == exitCol) || (exitCol + 1 == blockLeft);
    }

    if (ex.getOrientation() == 'H') {
        const int exitRow = ex.getX();
        const int exitLeft = ex.getY();
        const int exitRight = ex.getY() + len - 1;

        if (block.getWidth() > len) {
            return false;
        }
        if (blockLeft < exitLeft || blockRight > exitRight) {
            return false;
        }

        return (blockBottom + 1 == exitRow) || (exitRow + 1 == blockTop);
    }

    return false;
}

bool GameState::tryGateCross(const Block& block, int direction, Block& outBlock) const {
    if (block.getColorLock() > 0) {
        return false;
    }

    for (int g = 0; g < board.getGateCount(); ++g) {
        const Gate& gate = board.getGates()[g];
        if (gate.getColorAtStep(currentStep) != block.getColor()) {
            continue;
        }

        Block candidate = block;

        if (direction == DIR_RIGHT && gate.getOrientation() == 'V') {
            if (gate.getY() != block.getY() + block.getWidth()) {
                continue;
            }
            if (gate.getX() < block.getX() || gate.getX() > block.getX() + block.getHeight() - 1) {
                continue;
            }
            candidate.setY(gate.getY() + 1);
        } else if (direction == DIR_LEFT && gate.getOrientation() == 'V') {
            if (gate.getY() != block.getY() - 1) {
                continue;
            }
            if (gate.getX() < block.getX() || gate.getX() > block.getX() + block.getHeight() - 1) {
                continue;
            }
            candidate.setY(gate.getY() - block.getWidth());
        } else if (direction == DIR_DOWN && gate.getOrientation() == 'H') {
            if (gate.getX() != block.getX() + block.getHeight()) {
                continue;
            }
            if (gate.getY() < block.getY() || gate.getY() > block.getY() + block.getWidth() - 1) {
                continue;
            }
            candidate.setX(gate.getX() + 1);
        } else if (direction == DIR_UP && gate.getOrientation() == 'H') {
            if (gate.getX() != block.getX() - 1) {
                continue;
            }
            if (gate.getY() < block.getY() || gate.getY() > block.getY() + block.getWidth() - 1) {
                continue;
            }
            candidate.setX(gate.getX() - block.getHeight());
        } else {
            continue;
        }

        if (isValidPlacement(candidate, block.getId())) {
            outBlock = candidate;
            return true;
        }
    }

    return false;
}

bool GameState::isGoal() const {
    for (int i = 0; i < blockCount; ++i) {
        if (!blocksExited[i]) {
            return false;
        }
    }
    return true;
}

unsigned long GameState::hash() const {
    unsigned long h = 1469598103u;
    for (int i = 0; i < blockCount; ++i) {
        h = h * 31u + static_cast<unsigned long>(blocks[i].getId());
        h = h * 31u + static_cast<unsigned long>(blocks[i].getX() + 1000);
        h = h * 31u + static_cast<unsigned long>(blocks[i].getY() + 1000);
        h = h * 31u + static_cast<unsigned long>(blocksExited[i] ? 1 : 0);
        h = h * 31u + static_cast<unsigned long>(blocks[i].getColorLock());
    }
    return h;
}

bool GameState::equals(const GameState& other) const {
    if (blockCount != other.blockCount || currentStep != other.currentStep) {
        return false;
    }

    for (int i = 0; i < blockCount; ++i) {
        const int otherIndex = other.findBlockIndexById(blocks[i].getId());
        if (otherIndex < 0) {
            return false;
        }
        if (blocksExited[i] != other.blocksExited[otherIndex]) {
            return false;
        }
        if (blocks[i].getX() != other.blocks[otherIndex].getX() ||
            blocks[i].getY() != other.blocks[otherIndex].getY() ||
            blocks[i].getColorLock() != other.blocks[otherIndex].getColorLock()) {
            return false;
        }
    }

    return true;
}

int GameState::getStep() const { return currentStep; }
int GameState::getBlockCount() const { return blockCount; }
const Board& GameState::getBoard() const { return board; }
const Block& GameState::getBlock(int index) const { return blocks[index]; }

bool GameState::hasBlockExited(int blockId) const {
    const int index = findBlockIndexById(blockId);
    return index >= 0 && blocksExited[index];
}

void GameState::advanceStep() {
    ++currentStep;
}

void GameState::print() const {
    std::cout << "Paso: " << currentStep << " | Bloques fuera: ";
    for (int i = 0; i < blockCount; ++i) {
        if (blocksExited[i]) {
            std::cout << blocks[i].getId() << " ";
        }
    }
    std::cout << '\n';
}
