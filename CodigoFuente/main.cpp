#include <ctime>
#include <cstring>
#include <iostream>

#include "AStarSolver.h"
#include "FileParser.h"
#include "GameState.h"

class ConsoleApp {
public:
    static int run(int argc, char** argv) {
        printHeader();

        if (argc > 1) {
            for (int i = 1; i < argc; ++i) {
                solveFile(argv[i]);
            }
            return 0;
        }

        showMenu();
        return 0;
    }

private:
    static void printHeader() {
        std::cout << "========================================\n";
        std::cout << "      COLOR BLOCK JAM - SOLVER A*\n";
        std::cout << "========================================\n";
    }

    static void showMenu() {
        int option = -1;

        while (option != 0) {
            std::cout << "\nMenu principal\n";
            std::cout << "1. Resolver archivo por ruta\n";
            std::cout << "2. Resolver ejemplo simple1.txt\n";
            std::cout << "3. Resolver ejemplo facil1_corregido.txt\n";
            std::cout << "4. Resolver todos los ejemplos conocidos\n";
            std::cout << "0. Salir\n";
            std::cout << "Seleccione una opcion: ";

            if (!(std::cin >> option)) {
                std::cin.clear();
                discardLine();
                std::cout << "Entrada invalida.\n";
                continue;
            }
            discardLine();

            if (option == 1) {
                char path[512];
                std::cout << "Ruta del archivo: ";
                std::cin.getline(path, sizeof(path));
                if (std::strlen(path) == 0) {
                    std::cout << "Ruta vacia.\n";
                    continue;
                }
                solveFile(path);
            } else if (option == 2) {
                solveFile("simple1.txt");
            } else if (option == 3) {
                solveFile("facil1_corregido.txt");
            } else if (option == 4) {
                solveFile("simple1.txt");
                solveFile("facil1_corregido.txt");
            } else if (option == 0) {
                std::cout << "Saliendo.\n";
            } else {
                std::cout << "Opcion no reconocida.\n";
            }
        }
    }

    static void discardLine() {
        std::cin.ignore(10000, '\n');
    }

    static void solveFile(const char* filename) {
        FileParser::ParsedLevel level;

        std::cout << "\n----------------------------------------\n";
        std::cout << "Archivo: " << filename << '\n';
        std::cout << "Cargando nivel...\n";

        if (!FileParser::loadLevel(filename, level)) {
            std::cout << "ERROR: No se pudo cargar el archivo.\n";
            return;
        }

        GameState initialState(*level.board, level.blocks, level.blockCount);

        std::cout << "Resolviendo con A*...\n";
        const clock_t startTime = clock();
        AStarSolver::Result result = AStarSolver::solve(initialState, 5000000, level.stepLimit);
        const clock_t endTime = clock();
        const double elapsedMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

        printResult(result, elapsedMs);

        if (result.found) {
            GameState::Move* solutionMoves = copyMovesToArray(result);
            std::cout << "\nMovimientos guardados en arreglo: "
                      << result.moveCount << "\n";
            replaySolution(initialState, solutionMoves, result.moveCount);
            delete[] solutionMoves;
        }

        AStarSolver::freeResult(result);
        FileParser::freeLevel(level);
    }

    static void printResult(const AStarSolver::Result& result, double elapsedMs) {
        std::cout << "\n================ RESULTADOS ================\n";
        std::cout << "Tiempo de ejecucion: " << elapsedMs << " ms\n";
        std::cout << "Nodos expandidos: " << result.expanded << '\n';
        std::cout << "Nodos generados: " << result.generated << '\n';

        if (!result.found) {
            std::cout << "Juego sin solucion.\n";
            std::cout << "============================================\n";
            return;
        }

        std::cout << "Solucion encontrada.\n";
        std::cout << "Cantidad de movimientos: " << result.moveCount << '\n';
        std::cout << "Pasos:\n";

        for (int i = 0; i < result.moveCount; ++i) {
            const GameState::Move move = result.moves[i];
            std::cout << directionToChar(move.direction)
                      << move.blockId << "," << move.distance;
            if (i < result.moveCount - 1) {
                std::cout << "; ";
            }
        }

        std::cout << "\n============================================\n";
    }

    static char directionToChar(int direction) {
        if (direction == GameState::DIR_UP) return 'U';
        if (direction == GameState::DIR_DOWN) return 'D';
        if (direction == GameState::DIR_LEFT) return 'L';
        if (direction == GameState::DIR_RIGHT) return 'R';
        return '?';
    }

    static GameState::Move* copyMovesToArray(const AStarSolver::Result& result) {
        if (result.moveCount <= 0) {
            return nullptr;
        }

        GameState::Move* moves = new GameState::Move[result.moveCount];
        for (int i = 0; i < result.moveCount; ++i) {
            moves[i] = result.moves[i];
        }
        return moves;
    }

    static void replaySolution(const GameState& initialState,
                               const GameState::Move* moves,
                               int moveCount) {
        GameState current(initialState);

        std::cout << "\nReproduccion grafica paso a paso\n";
        std::cout << "Estado inicial:\n";
        printBoard(current);

        for (int i = 0; i < moveCount; ++i) {
            waitForEnter();

            const GameState::Move move = moves[i];
            std::cout << "\nMovimiento " << (i + 1) << "/" << moveCount << ": "
                      << directionToChar(move.direction)
                      << move.blockId << "," << move.distance << '\n';

            if (!current.applyMove(move)) {
                std::cout << "ERROR: No se pudo aplicar este movimiento en la reproduccion.\n";
                return;
            }

            printBoard(current);
        }

        std::cout << "\nFin de la reproduccion.\n";
    }

    static void waitForEnter() {
        char line[8];
        std::cout << "\nPresione Enter para ver el siguiente movimiento...";
        std::cin.getline(line, sizeof(line));
        if (!std::cin.good()) {
            std::cin.clear();
        }
    }

    static void printBoard(const GameState& state) {
        const Board& board = state.getBoard();

        std::cout << "\nPaso de tiempo: " << state.getStep() << '\n';
        std::cout << "   ";
        for (int col = 0; col < board.getWidth(); ++col) {
            std::cout << (col % 10);
        }
        std::cout << '\n';

        for (int row = 0; row < board.getHeight(); ++row) {
            std::cout << (row % 10) << "  ";
            for (int col = 0; col < board.getWidth(); ++col) {
                std::cout << cellCharAt(state, row, col);
            }
            std::cout << '\n';
        }

        std::cout << "Leyenda: # pared, MAYUSCULA bloque, minuscula salida/compuerta, . vacio\n";
    }

    static char cellCharAt(const GameState& state, int row, int col) {
        const char blockChar = blockCharAt(state, row, col);
        if (blockChar != '\0') {
            return blockChar;
        }

        const char gateChar = gateCharAt(state, row, col);
        if (gateChar != '\0') {
            return gateChar;
        }

        const char exitChar = exitCharAt(state, row, col);
        if (exitChar != '\0') {
            return exitChar;
        }

        if (state.getBoard().isWallAt(row, col)) {
            return '#';
        }

        return '.';
    }

    static char blockCharAt(const GameState& state, int row, int col) {
        for (int i = 0; i < state.getBlockCount(); ++i) {
            const Block& block = state.getBlock(i);
            if (!state.hasBlockExited(block.getId()) && block.occupiesCell(row, col)) {
                if (block.getColorLock() > 0) {
                    return '?';
                }
                return toUpper(block.getColor());
            }
        }
        return '\0';
    }

    static char gateCharAt(const GameState& state, int row, int col) {
        const Board& board = state.getBoard();
        for (int i = 0; i < board.getGateCount(); ++i) {
            const Gate& gate = board.getGates()[i];
            if (gate.getX() == row && gate.getY() == col) {
                return gate.getColorAtStep(state.getStep());
            }
        }
        return '\0';
    }

    static char exitCharAt(const GameState& state, int row, int col) {
        const Board& board = state.getBoard();
        for (int i = 0; i < board.getExitCount(); ++i) {
            const Exit& exitObj = board.getExits()[i];
            const int length = exitObj.getLengthAtStep(state.getStep());

            if (exitObj.getOrientation() == 'V') {
                if (col == exitObj.getY() &&
                    row >= exitObj.getX() &&
                    row < exitObj.getX() + length) {
                    return exitObj.getColor();
                }
            } else if (exitObj.getOrientation() == 'H') {
                if (row == exitObj.getX() &&
                    col >= exitObj.getY() &&
                    col < exitObj.getY() + length) {
                    return exitObj.getColor();
                }
            }
        }
        return '\0';
    }

    static char toUpper(char value) {
        if (value >= 'a' && value <= 'z') {
            return static_cast<char>(value - 'a' + 'A');
        }
        return value;
    }
};

int main(int argc, char** argv) {
    return ConsoleApp::run(argc, argv);
}
