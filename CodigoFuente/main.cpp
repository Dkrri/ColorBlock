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
        while (true) {
            std::cout << "\nMenu principal\n";
            std::cout << "1. Evaluar tablero\n";
            std::cout << "2. Salir\n";
            std::cout << "Seleccione una opcion: ";

            char input[32];
            std::cin.getline(input, sizeof(input));
            if (!std::cin.good()) {
                std::cin.clear();
                discardLine();
                std::cout << "Entrada invalida.\n";
                continue;
            }

            if (input[0] == '1' && input[1] == '\0') {
                showEvaluateBoardMenu();
            } else if (input[0] == '2' && input[1] == '\0') {
                std::cout << "Saliendo.\n";
                break;
            } else {
                std::cout << "Opcion no reconocida.\n";
            }
        }
    }

    static void showEvaluateBoardMenu() {
        while (true) {
            std::cout << "\nEvaluar tablero\n";
            std::cout << "0. Ruta de tablero\n";
            std::cout << "1. facil1_corregido\n";
            std::cout << "2. facil2_corregido\n";
            std::cout << "3. facil3\n";
            std::cout << "4. facil4\n";
            std::cout << "5. facil5\n";
            std::cout << "6. mediano1\n";
            std::cout << "7. dificil1\n";
            std::cout << "x. Salir\n";
            std::cout << "Seleccione una opcion: ";

            char input[64];
            std::cin.getline(input, sizeof(input));
            if (!std::cin.good()) {
                std::cin.clear();
                discardLine();
                std::cout << "Entrada invalida.\n";
                continue;
            }

            if (input[0] == 'x' && input[1] == '\0') {
                break;
            }

            if (input[0] == '0' && input[1] == '\0') {
                char path[512];
                std::cout << "Ruta de tablero: ";
                std::cin.getline(path, sizeof(path));
                if (std::strlen(path) == 0) {
                    std::cout << "Ruta vacia.\n";
                    continue;
                }
                solveFile(path);
            } else if (input[0] == '1' && input[1] == '\0') {
                solveFile("facil1_corregido.txt");
            } else if (input[0] == '2' && input[1] == '\0') {
                solveFile("facil2_corregido.txt");
            } else if (input[0] == '3' && input[1] == '\0') {
                solveFile("facil3.txt");
            } else if (input[0] == '4' && input[1] == '\0') {
                solveFile("facil4.txt");
            } else if (input[0] == '5' && input[1] == '\0') {
                solveFile("facil5.txt");
            } else if (input[0] == '6' && input[1] == '\0') {
                solveFile("mediano1.txt");
            } else if (input[0] == '7' && input[1] == '\0') {
                solveFile("dificil1.txt");
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

        if (result.found) {
            GameState::Move* solutionMoves = copyMovesToArray(result);
            std::cout << "\nMovimientos guardados en arreglo: "
                      << result.moveCount << "\n";
            replaySolution(initialState, solutionMoves, result.moveCount);
            delete[] solutionMoves;
        }

        // Report summary after the final board step has been shown.
        printResult(result, elapsedMs);

        AStarSolver::freeResult(result);
        FileParser::freeLevel(level);
    }

    static void runManualSequence() {
        char path[512];
        char sequence[2048];

        std::cout << "Ruta del archivo: ";
        std::cin.getline(path, sizeof(path));
        if (std::strlen(path) == 0) {
            std::cout << "Ruta vacia.\n";
            return;
        }

        std::cout << "Secuencia (ej: R1,1; D2,3; L1,2): ";
        std::cin.getline(sequence, sizeof(sequence));
        if (std::strlen(sequence) == 0) {
            std::cout << "Secuencia vacia.\n";
            return;
        }

        GameState::Move moves[256];
        int moveCount = parseMoveSequence(sequence, moves, 256);
        if (moveCount <= 0) {
            std::cout << "No se pudo interpretar la secuencia.\n";
            return;
        }

        std::cout << "Movimientos guardados en arreglo: " << moveCount << '\n';
        replayManualMoves(path, moves, moveCount);
    }

    static void printResult(const AStarSolver::Result& result, double elapsedMs) {
        if (!result.found) {
            std::cout << "no se encontro solucion\n";
            return;
        }

        const int elapsedMsInt = static_cast<int>(elapsedMs + 0.5);
        std::cout << "Tiempo resolucion: " << elapsedMsInt << "[mseg]\n";
        std::cout << "Solucion encontrada.\n";
        std::cout << "Pasos:\n";

        for (int i = 0; i < result.moveCount; ++i) {
            const GameState::Move move = result.moves[i];
            std::cout << directionToChar(move.direction)
                      << move.blockId << "," << move.distance << '\n';
        }
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

    static int parseMoveSequence(const char* text,
                                 GameState::Move* moves,
                                 int maxMoves) {
        int count = 0;
        int pos = 0;

        while (text[pos] != '\0' && count < maxMoves) {
            skipSeparators(text, pos);
            if (text[pos] == '\0') {
                break;
            }

            GameState::Move move;
            if (!parseOneMove(text, pos, move)) {
                return 0;
            }
            moves[count++] = move;
            skipSeparators(text, pos);
        }

        return count;
    }

    static bool parseOneMove(const char* text, int& pos, GameState::Move& move) {
        const char direction = text[pos++];
        move.direction = charToDirection(direction);
        if (move.direction < 0) {
            return false;
        }

        if (!parsePositiveInt(text, pos, move.blockId)) {
            return false;
        }

        if (text[pos] != ',') {
            return false;
        }
        ++pos;

        if (!parsePositiveInt(text, pos, move.distance)) {
            return false;
        }

        return true;
    }

    static bool parsePositiveInt(const char* text, int& pos, int& value) {
        if (text[pos] < '0' || text[pos] > '9') {
            return false;
        }

        value = 0;
        while (text[pos] >= '0' && text[pos] <= '9') {
            value = (value * 10) + (text[pos] - '0');
            ++pos;
        }

        return value > 0;
    }

    static void skipSeparators(const char* text, int& pos) {
        while (text[pos] == ' ' || text[pos] == '\t' ||
               text[pos] == ';' || text[pos] == '\n' ||
               text[pos] == '\r') {
            ++pos;
        }
    }

    static int charToDirection(char direction) {
        if (direction == 'U' || direction == 'u') return GameState::DIR_UP;
        if (direction == 'D' || direction == 'd') return GameState::DIR_DOWN;
        if (direction == 'L' || direction == 'l') return GameState::DIR_LEFT;
        if (direction == 'R' || direction == 'r') return GameState::DIR_RIGHT;
        return -1;
    }

    static void replayManualMoves(const char* filename,
                                  const GameState::Move* moves,
                                  int moveCount) {
        FileParser::ParsedLevel level;
        if (!FileParser::loadLevel(filename, level)) {
            std::cout << "ERROR: No se pudo cargar el archivo.\n";
            return;
        }

        GameState initialState(*level.board, level.blocks, level.blockCount);
        replaySolution(initialState, moves, moveCount);
        FileParser::freeLevel(level);
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

        for (int row = 0; row < board.getHeight(); ++row) {
            for (int col = 0; col < board.getWidth(); ++col) {
                std::cout << cellCharAt(state, row, col);
            }
            std::cout << '\n';
        }

        std::cout << "Leyenda: # pared, minuscula bloque, MAYUSCULA salida, . vacio\n";
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

        return ' ';
    }

    static char blockCharAt(const GameState& state, int row, int col) {
        for (int i = 0; i < state.getBlockCount(); ++i) {
            const Block& block = state.getBlock(i);
            if (!state.hasBlockExited(block.getId()) && block.occupiesCell(row, col)) {
                if (block.getColorLock() > 0) {
                    return '?';
                }
                return toLower(block.getColor());
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
                    return toUpper(exitObj.getColor());
                }
            } else if (exitObj.getOrientation() == 'H') {
                if (row == exitObj.getX() &&
                    col >= exitObj.getY() &&
                    col < exitObj.getY() + length) {
                    return toUpper(exitObj.getColor());
                }
            }
        }
        return '\0';
    }

    static char toLower(char value) {
        if (value >= 'A' && value <= 'Z') {
            return static_cast<char>(value - 'A' + 'a');
        }
        return value;
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
