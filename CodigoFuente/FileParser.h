#pragma once

#include "Board.h"
#include "Block.h"

class FileParser {
public:
    struct ParsedLevel {
        Board* board;
        Block* blocks;
        bool** geometries;
        int blockCount;
        int stepLimit;
        int width;
        int height;
    };

    // Carga un nivel desde archivo de texto.
    // Retorna true si el parseo fue exitoso.
    static bool loadLevel(const char* filePath, ParsedLevel& outLevel);

    // Libera memoria asociada al nivel cargado.
    static void freeLevel(ParsedLevel& level);

private:
    static void resetParsedLevel(ParsedLevel& level);
    static void trimLine(char* line);
    static void trimLineRight(char* line);
    static bool isEmptyOrComment(const char* line);
    static bool parseMetaLine(const char* line, int& width, int& height, int& stepLimit);
    static bool parseBlockLine(
        const char* line,
        int& id,
        int& colorLock,
        char& color,
        int& w,
        int& h,
        int& x,
        int& y,
        char* mask,
        int maskSize,
        bool& hasMask);
    static bool applyMaskToGeometry(const char* mask, int w, int h, bool* geometry);
    static bool parseWallLine(const char* line, int& x, int& y, bool& isExit, bool& isGate);
    static bool parseExitLine(const char* line, int& x, int& y, char& color, char& ori, int& li, int& lf, int& step);
    static bool parseGateLine(const char* line, int& x, int& y, char& ori, char& ci, char& cf, int& step);
    static bool isSection(const char* line, const char* sectionName);
};
