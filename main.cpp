#include <iostream>

#include "Block.h"
#include "Board.h"
#include "Exit.h"
#include "Gate.h"

int main() {
    // Tablero base para prueba rapida.
    Board board(6, 6);

    // Geometria 2x2 completamente llena.
    bool geometryA[] = {
        true, true,
        true, true
    };

    // Geometria 2x2 en forma de L.
    bool geometryB[] = {
        true, false,
        true, true
    };

    Block blockA(1, 'R', 2, 2, 1, 1, geometryA);
    Block blockB(2, 'G', 2, 2, 2, 2, geometryB);

    std::cout << "Colocar blockA: " << (board.placeBlock(blockA) ? "ok" : "fallo") << '\n';
    std::cout << "Colocar blockB: " << (board.placeBlock(blockB) ? "ok" : "fallo") << '\n';

    board.print();

    Exit exitObj(5, 2, 'R', 'H', 1, 3, 1);
    Gate gateObj(0, 0, 'V', 'R', 'G', 2);

    std::cout << "Largo de salida en paso 2: " << exitObj.getLengthAtStep(2) << '\n';
    std::cout << "Color de gate en paso 1: " << gateObj.getColorAtStep(1) << '\n';
    std::cout << "Color de gate en paso 3: " << gateObj.getColorAtStep(3) << '\n';

    return 0;
}
