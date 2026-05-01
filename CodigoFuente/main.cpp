#include <iostream>

#include "FileParser.h"
#include "GameState.h"

int main(int argc, char* argv[]) {
    const char* levelPath = "nivel_demo.txt";
    if (argc > 1) {
        levelPath = argv[1];
    }

    FileParser::ParsedLevel level;
    if (!FileParser::loadLevel(levelPath, level)) {
        std::cout << "No se pudo cargar el nivel desde: " << levelPath << '\n';
        std::cout << "Uso: app.exe [ruta_archivo_nivel]\n";
        return 1;
    }

    std::cout << "=== Nivel cargado ===\n";
    std::cout << "Archivo: " << levelPath << '\n';
    std::cout << "Ancho: " << level.width << " Alto: " << level.height << '\n';
    std::cout << "Bloques: " << level.blockCount << " Step limit: " << level.stepLimit << "\n\n";

    std::cout << "Tablero inicial:\n";
    level.board->print();

    // Crear estado inicial del juego desde lo parseado.
    GameState gameState(*level.board, level.blocks, level.blockCount);

    std::cout << "\nEstado inicial del juego:\n";
    gameState.print();

    // Intentar generar movimientos para el bloque 1.
    std::cout << "\nGenerando movimientos para bloque 1...\n";
    GameState::Move movements[50];
    int moveCount = gameState.generateMovementsForBlock(1, movements, 50);
    std::cout << "Movimientos disponibles: " << moveCount << '\n';
    for (int i = 0; i < moveCount && i < 5; ++i) {
        std::cout << "  Movimiento " << i << ": Dir=" << movements[i].direction
                  << " Distancia=" << movements[i].distance << '\n';
    }

    // Verificar si es estado objetivo.
    std::cout << "\nEs estado objetivo (todos bloques salieron)? "
              << (gameState.isGoal() ? "Si" : "No") << '\n';

    // Calcular hash del estado.
    std::cout << "Hash del estado: " << gameState.hash() << '\n';

    FileParser::freeLevel(level);

    return 0;
}
