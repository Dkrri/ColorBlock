#include "GameState.h"

#include <iostream>
#include <cstring>

GameState::GameState(const Board& initialBoard, Block* initialBlocks, int blockCount)
    : board(initialBoard),
      blockCount(blockCount),
      currentStep(0),
      maxBlocks(blockCount) {
    blocks = new Block[maxBlocks];
    for (int i = 0; i < blockCount; ++i) {
        blocks[i] = initialBlocks[i];
    }
    blocksExited = new bool[maxBlocks];
    for (int i = 0; i < maxBlocks; ++i) {
        blocksExited[i] = false;
    }
}

GameState::GameState(const GameState& other)
    : board(other.board),
      blockCount(other.blockCount),
      currentStep(other.currentStep),
      maxBlocks(other.maxBlocks) {
    blocks = new Block[maxBlocks];
    for (int i = 0; i < blockCount; ++i) {
        blocks[i] = other.blocks[i];
    }
    blocksExited = new bool[maxBlocks];
    for (int i = 0; i < maxBlocks; ++i) {
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
    for (int i = 0; i < blockCount; ++i) {
        blocks[i] = other.blocks[i];
    }
    blocksExited = new bool[maxBlocks];
    for (int i = 0; i < maxBlocks; ++i) {
        blocksExited[i] = other.blocksExited[i];
    }
    return *this;
}

GameState::~GameState() {
    delete[] blocks;
    delete[] blocksExited;
}

int GameState::getStep() const {
    return currentStep;
}

int GameState::getBlockCount() const {
    return blockCount;
}

const Board& GameState::getBoard() const {
    return board;
}

const Block& GameState::getBlock(int index) const {
    return blocks[index];
}

bool GameState::hasBlockExited(int blockId) const {
    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getId() == blockId) {
            return blocksExited[i];
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

bool GameState::isValidPlacement(const Block& block, int ignoreBlockId) const {
    const int bx = block.getX();
    const int by = block.getY();
    const int bw = block.getWidth();
    const int bh = block.getHeight();
    const bool* bgeo = block.getGeometry();

    for (int y = 0; y < bh; ++y) {
        for (int x = 0; x < bw; ++x) {
            bool filled = (bgeo == nullptr) ? true : bgeo[y * bw + x];
            if (!filled) continue;

            int wx = bx + x;
            int wy = by + y;

            if (!board.isInside(wx, wy)) return false;
            
            // Check board walls directly
            if (board.isWallAt(wx, wy)) return false;

            // Check against other unexited blocks
            for (int i = 0; i < blockCount; ++i) {
                if (blocksExited[i] || blocks[i].getId() == ignoreBlockId) continue;
                if (blocks[i].occupiesCell(wx, wy)) return false;
            }
        }
    }
    return true;
}

int GameState::checkAdjacentExitOrGate(const Block& block, bool& isExitTrig, int& gateJumpDist) const {
    isExitTrig = false;
    gateJumpDist = 0;
    
    if (block.getColorLock() > 0) return 0;

    const bool* bgeo = block.getGeometry();
    const int bx = block.getX();
    const int by = block.getY();
    const int bw = block.getWidth();
    const int bh = block.getHeight();

    auto isBlockCell = [&](int x, int y) -> bool {
        if (x < bx || x >= bx + bw || y < by || y >= by + bh) return false;
        if (!bgeo) return true;
        return bgeo[(y - by) * bw + (x - bx)];
    };

    auto isBlockAdjacentToXY = [&](int x, int y) -> bool {
        return isBlockCell(x - 1, y) || isBlockCell(x + 1, y) ||
               isBlockCell(x, y - 1) || isBlockCell(x, y + 1);
    };

    // Verificar salidas
    for (int i = 0; i < board.getExitCount(); ++i) {
        const Exit& ex = board.getExits()[i];
        if (ex.getColor() == block.getColor() && isBlockAdjacentToXY(ex.getX(), ex.getY())) {
            // Verificar si el tamanio actual incide
            int currentLength = ex.getLengthAtStep(currentStep);
            if (bw <= currentLength || bh <= currentLength) {
                isExitTrig = true;
                return 1;
            }
        }
    }

    // Verificar compuertas
    for (int i = 0; i < board.getGateCount(); ++i) {
        const Gate& gt = board.getGates()[i];
        if (gt.getColorAtStep(currentStep) == block.getColor() && isBlockAdjacentToXY(gt.getX(), gt.getY())) {
            // Evaluamos un "salto" basico de 3 celdas (pasar la pared por completo) 
            // siempre que quepa al otro lado, lo validaremos luego en isValidPlacement.
            gateJumpDist = 3; 
            return 2;
        }
    }

    return 0; 
}

int GameState::generateMovementsForBlock(int blockId, Move* movements, int maxMovements) const {
    int movementCount = 0;
    int blockIndex = -1;
    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getId() == blockId) {
            blockIndex = i;
            break;
        }
    }

    if (blockIndex < 0 || blocksExited[blockIndex]) return 0;
    const Block& block = blocks[blockIndex];

    // Primero verificamos si ESTA adyacente a una puerta o salida antes de moverse
    bool isExitTrig = false;
    int gateJump = 0;
    int adjRes = checkAdjacentExitOrGate(block, isExitTrig, gateJump);
    if (adjRes == 1 && isExitTrig) {
        // Generar un movimiento ficticio con distancia 0 para denotar una "SALIDA" directa.
        // Opcional: distancia especial 99 para notar que el bloque saldra inmediatamente.
        movements[movementCount++] = {blockId, DIR_UP, 99}; 
        return movementCount;
    }

    int directions[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    for (int dirIdx = 0; dirIdx < DIR_COUNT && movementCount < maxMovements; ++dirIdx) {
        int dir = directions[dirIdx];

        for (int distance = 1; distance <= 10; ++distance) {
            Block testBlock = block;
            if (dir == DIR_UP) testBlock.moveBy(0, -distance);
            else if (dir == DIR_DOWN) testBlock.moveBy(0, distance);
            else if (dir == DIR_LEFT) testBlock.moveBy(-distance, 0);
            else if (dir == DIR_RIGHT) testBlock.moveBy(distance, 0);

            // Validacion Fisica Estricta (si choca con otra pared, limite o bloque no exited: falso)
            bool isClear = isValidPlacement(testBlock, blockId);

            if (isClear) {
                // Posicionar bloque valido en ruta disponible
                movements[movementCount++] = {blockId, dir, distance};
                
                // Si esta nueva posicion nos deja adyacentes a una salida, reportarlo opcionalmente
                // En A* evaluamos isGoal en la expansion luego.
            } else {
                // Chocamos con una pared fuerte o bloque (o salimos de bounds)
                // Chequemos si es una puerta/salida a traves de la cual podiamos "saltar"?
                // Para simplificar: el bloque debe estar adyacente a la puerta en el paso previo (distance-1)
                // y ejecutar un testBlock saltando completamente la puerta y chequear si esClear ese salto.
                
                break; // Rompemos este dir porque no atravesamos muros duros en movimiento continuo
            }
        }
    }

    // Opcion de puerta (si estamos prev. adyacentes a una puerta y podemos caer al otro lado)
    if (adjRes == 2) {
        // En una implementacion A* real de Board, se probaria poner testBlock a (x +- gateJump).
        // Por ahora, lo mantenemos modular y funcional al core.
    }

    return movementCount;
}

bool GameState::applyMove(const Move& move) {
    int blockIndex = -1;
    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getId() == move.blockId) {
            blockIndex = i;
            break;
        }
    }

    if (blockIndex < 0 || blocksExited[blockIndex]) return false;

    Block& block = blocks[blockIndex];

    if (move.distance == 99) {
        // Movimiento de salida! El bloque se consume y desaparece
        blocksExited[blockIndex] = true;
        board.removeBlock(block); // Lo borramos explicitamente de los ID de las celdas
        
        // Decriminar lock de todos los demas bloques al salir alguien
        for (int i = 0; i < blockCount; ++i) {
            if (!blocksExited[i]) {
                blocks[i].decrementColorLock();
            }
        }
        return true;
    }

    board.removeBlock(block);

    if (move.direction == DIR_UP) block.moveBy(0, -move.distance);
    else if (move.direction == DIR_DOWN) block.moveBy(0, move.distance);
    else if (move.direction == DIR_LEFT) block.moveBy(-move.distance, 0);
    else if (move.direction == DIR_RIGHT) block.moveBy(move.distance, 0);

    if (!isValidPlacement(block, block.getId())) {
        return false;
    }

    board.placeBlock(block);
    currentStep += move.distance;

    return true;
}

bool GameState::undoMove(const Move& move) {
    // Encontrar el indice del bloque.
    int blockIndex = -1;
    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getId() == move.blockId) {
            blockIndex = i;
            break;
        }
    }

    if (blockIndex < 0) {
        return false;
    }

    Block& block = blocks[blockIndex];

    // Remover el bloque del tablero.
    board.removeBlock(block);

    // Aplicar movimiento inverso.
    if (move.direction == DIR_UP) {
        block.moveBy(0, move.distance);
    } else if (move.direction == DIR_DOWN) {
        block.moveBy(0, -move.distance);
    } else if (move.direction == DIR_LEFT) {
        block.moveBy(move.distance, 0);
    } else if (move.direction == DIR_RIGHT) {
        block.moveBy(-move.distance, 0);
    }

    // Recolocar el bloque.
    board.placeBlock(block);

    // Retroceder el paso.
    currentStep -= move.distance;

    return true;
}

unsigned long GameState::hash() const {
    unsigned long hashValue = 0;

    // Combinar hash del step actual.
    hashValue = hashValue * 31 + currentStep;

    // Combinar posicion de cada bloque.
    for (int i = 0; i < blockCount; ++i) {
        hashValue = hashValue * 31 + blocks[i].getX();
        hashValue = hashValue * 31 + blocks[i].getY();
        hashValue = hashValue * 31 + (blocksExited[i] ? 1 : 0);
    }

    return hashValue;
}

bool GameState::equals(const GameState& other) const {
    if (blockCount != other.blockCount || currentStep != other.currentStep) {
        return false;
    }

    for (int i = 0; i < blockCount; ++i) {
        if (blocks[i].getX() != other.blocks[i].getX() ||
            blocks[i].getY() != other.blocks[i].getY() ||
            blocksExited[i] != other.blocksExited[i]) {
            return false;
        }
    }

    return true;
}

void GameState::advanceStep() {
    ++currentStep;
}

void GameState::print() const {
    std::cout << "=== Estado del Juego ===\n";
    std::cout << "Paso: " << currentStep << '\n';
    std::cout << "Bloques:\n";
    for (int i = 0; i < blockCount; ++i) {
        std::cout << "  Bloque " << blocks[i].getId()
                  << " en (" << blocks[i].getX() << ", " << blocks[i].getY() << ")"
                  << " Color: " << blocks[i].getColor();
        if (blocksExited[i]) {
            std::cout << " [SALIDO]";
        }
        std::cout << '\n';
    }
}
