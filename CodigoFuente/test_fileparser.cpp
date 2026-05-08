#include <iostream>
#include "FileParser.h"

int main() {
    FileParser::ParsedLevel level;
    if (!FileParser::loadLevel("simple1.txt", level)) return 1;
    if (level.width != 8 || level.height != 8) return 1;
    if (level.blockCount != 1) return 1;
    if (level.board == nullptr || level.blocks == nullptr) return 1;
    if (level.board->getExitCount() != 1) return 1;

    FileParser::freeLevel(level);
    if (level.board != nullptr || level.blocks != nullptr) return 1;

    std::cout << "test_fileparser OK\n";
    return 0;
}
