#include "FileParser.h"

#include <cstdio>
#include <cstring>

void FileParser::resetParsedLevel(ParsedLevel& level) {
    level.board = nullptr;
    level.blocks = nullptr;
    level.geometries = nullptr;
    level.blockCount = 0;
    level.stepLimit = 0;
    level.width = 0;
    level.height = 0;
}

void FileParser::trimLine(char* line) {
    int len = static_cast<int>(std::strlen(line));
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' || line[len - 1] == ' ' || line[len - 1] == '\t')) {
        line[len - 1] = '\0';
        --len;
    }

    int start = 0;
    while (line[start] == ' ' || line[start] == '\t') {
        ++start;
    }

    if (start > 0) {
        int i = 0;
        while (line[start + i] != '\0') {
            line[i] = line[start + i];
            ++i;
        }
        line[i] = '\0';
    }
}

bool FileParser::isEmptyOrComment(const char* line) {
    return line[0] == '\0' || line[0] == '#';
}

bool FileParser::isSection(const char* line, const char* sectionName) {
    char section[32];
    std::snprintf(section, sizeof(section), "[%s]", sectionName);
    return std::strcmp(line, section) == 0;
}

bool FileParser::parseMetaLine(const char* line, int& width, int& height, int& stepLimit) {
    int value = 0;
    if (std::sscanf(line, "WIDTH=%d", &value) == 1) {
        width = value;
        return true;
    }
    if (std::sscanf(line, "HEIGHT=%d", &value) == 1) {
        height = value;
        return true;
    }
    if (std::sscanf(line, "STEP_LIMIT=%d", &value) == 1) {
        stepLimit = value;
        return true;
    }
    return false;
}

bool FileParser::parseBlockLine(
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
    bool& hasMask) {
    hasMask = false;
    if (maskSize > 0) {
        mask[0] = '\0';
    }

    if (std::sscanf(line, "%d,%d,%c,%d,%d,%d,%d,%127s", &id, &colorLock, &color, &w, &h, &x, &y, mask) == 8) {
        hasMask = true;
        return true;
    }

    if (std::sscanf(line, "%d,%d,%c,%d,%d,%d,%d", &id, &colorLock, &color, &w, &h, &x, &y) == 7) {
        return true;
    }

    return false;
}

bool FileParser::applyMaskToGeometry(const char* mask, int w, int h, bool* geometry) {
    const int expected = w * h;
    int index = 0;

    for (int i = 0; mask[i] != '\0'; ++i) {
        const char c = mask[i];

        if (c == '|' || c == '-' || c == '_' || c == ' ') {
            continue;
        }

        if (c != '0' && c != '1') {
            return false;
        }

        if (index >= expected) {
            return false;
        }

        geometry[index] = (c == '1');
        ++index;
    }

    return index == expected;
}

bool FileParser::parseWallLine(const char* line, int& x, int& y, bool& isExit, bool& isGate) {
    // Si la linea es de texto como "X=5,Y=3,TYPE=N", procesarla:
    char type = 'N';
    if (std::sscanf(line, "%d,%d,%c", &x, &y, &type) >= 2) {
        isExit = (type == 'E');
        isGate = (type == 'G');
        return true;
    }
    if (std::sscanf(line, "X=%d,Y=%d,TYPE=%c", &x, &y, &type) >= 2) {
        isExit = (type == 'E');
        isGate = (type == 'G');
        return true;
    }
    return false;
}

bool FileParser::parseExitLine(const char* line, int& x, int& y, char& color, char& ori, int& li, int& lf, int& step) {
    if (std::sscanf(line, "%d,%d,%c,%c,%d,%d,%d", &x, &y, &color, &ori, &li, &lf, &step) == 7) {
        return true;
    }
    if (std::sscanf(line, "X=%d,Y=%d,COLOR=%c,ORI=%c,LI=%d,LF=%d,STEP=%d", &x, &y, &color, &ori, &li, &lf, &step) == 7) {
        return true;
    }
    return false;
}

bool FileParser::parseGateLine(const char* line, int& x, int& y, char& ori, char& ci, char& cf, int& step) {
    if (std::sscanf(line, "%d,%d,%c,%c,%c,%d", &x, &y, &ori, &ci, &cf, &step) == 6) {
        return true;
    }
    if (std::sscanf(line, "X=%d,Y=%d,ORI=%c,CI=%c,CF=%c,STEP=%d", &x, &y, &ori, &ci, &cf, &step) == 6) {
        return true;
    }
    return false;
}

bool FileParser::loadLevel(const char* filePath, ParsedLevel& outLevel) {
    resetParsedLevel(outLevel);

    FILE* file = std::fopen(filePath, "r");
    if (file == nullptr) {
        return false;
    }

    char line[256];
    int width = 0;
    int height = 0;
    int stepLimit = 0;
    int blockCount = 0;

    char currentSection[16] = "";

    // Primera pasada: leer meta y contar bloques.
    while (std::fgets(line, sizeof(line), file) != nullptr) {
        trimLine(line);
        if (isEmptyOrComment(line)) {
            continue;
        }

        if (line[0] == '[') {
            std::snprintf(currentSection, sizeof(currentSection), "%s", line);
            continue;
        }

        if (std::strcmp(currentSection, "[META]") == 0) {
            parseMetaLine(line, width, height, stepLimit);
        } else if (std::strcmp(currentSection, "[BLOCK]") == 0) {
            int id = 0;
            int colorLock = 0;
            char color = ' ';
            int w = 0;
            int h = 0;
            int x = 0;
            int y = 0;
            char mask[128];
            bool hasMask = false;
            if (parseBlockLine(line, id, colorLock, color, w, h, x, y, mask, sizeof(mask), hasMask)) {
                ++blockCount;
            }
        }
    }

    if (width <= 0 || height <= 0 || blockCount <= 0) {
        std::fclose(file);
        return false;
    }

    outLevel.board = new Board(width, height);
    outLevel.blocks = new Block[blockCount];
    outLevel.geometries = new bool*[blockCount];
    outLevel.blockCount = blockCount;
    outLevel.stepLimit = stepLimit;
    outLevel.width = width;
    outLevel.height = height;

    for (int i = 0; i < blockCount; ++i) {
        outLevel.geometries[i] = nullptr;
    }

    std::rewind(file);
    currentSection[0] = '\0';
    int blockIndex = 0;
    int currentWallRowY = 0;

    // Segunda pasada: cargar entidades.
    while (std::fgets(line, sizeof(line), file) != nullptr) {
        trimLine(line);
        if (isEmptyOrComment(line)) {
            continue;
        }

        if (line[0] == '[') {
            std::snprintf(currentSection, sizeof(currentSection), "%s", line);
            continue;
        }

        if (std::strcmp(currentSection, "[BLOCK]") == 0) {
            int id = 0;
            int colorLock = 0;
            char color = ' ';
            int w = 0;
            int h = 0;
            int x = 0;
            int y = 0;
            char mask[128];
            bool hasMask = false;
            if (parseBlockLine(line, id, colorLock, color, w, h, x, y, mask, sizeof(mask), hasMask) && blockIndex < blockCount) {
                bool* geometry = new bool[w * h];
                for (int i = 0; i < (w * h); ++i) {
                    geometry[i] = true;
                }

                if (hasMask && !applyMaskToGeometry(mask, w, h, geometry)) {
                    delete[] geometry;
                    std::fclose(file);
                    freeLevel(outLevel);
                    return false;
                }

                outLevel.geometries[blockIndex] = geometry;
                outLevel.blocks[blockIndex] = Block(id, colorLock, color, w, h, x, y, geometry);
                outLevel.board->placeBlock(outLevel.blocks[blockIndex]);
                ++blockIndex;
            }
        } else if (std::strcmp(currentSection, "[WALL]") == 0) {
            int x = 0;
            int y = 0;
            bool isExit = false;
            bool isGate = false;
            
            // Evaluacion PDF: La secion WALL puede venir como matriz H x W
            // con valores '#' para pared y ' ' para espacio.
            // Si la linea detecta '#' o ' ', iterar la linea sumando a Y=0.., e iterando en X.
            // Para acoplarnos al código base actual pero preparar el soporte a matrices:
            if (line[0] == '#' || line[0] == ' ') {
                for (int currentX = 0; line[currentX] != '\0'; ++currentX) {
                    if (line[currentX] == '#') {
                        // isExit o isGate se inicializan en falso porque esto
                        // es solo una pared base, las salidas reales se declaran en [EXIT] o [GATE].
                        Wall wall(currentX, currentWallRowY, false, false);
                        outLevel.board->addWall(wall);
                    }
                }
                currentWallRowY++;
            } else if (parseWallLine(line, x, y, isExit, isGate)) {
                // Compatibilidad hacia atrás (Formato "X,Y,TYPE")
                Wall wall(x, y, isExit, isGate);
                outLevel.board->addWall(wall);
            }
        } else if (std::strcmp(currentSection, "[EXIT]") == 0) {
            int x = 0;
            int y = 0;
            char color = ' ';
            char ori = ' ';
            int li = 0;
            int lf = 0;
            int step = 0;
            if (parseExitLine(line, x, y, color, ori, li, lf, step)) {
                Exit exitObj(x, y, color, ori, li, lf, step);
                outLevel.board->addExit(exitObj);
            }
        } else if (std::strcmp(currentSection, "[GATE]") == 0) {
            int x = 0;
            int y = 0;
            char ori = ' ';
            char ci = ' ';
            char cf = ' ';
            int step = 0;
            if (parseGateLine(line, x, y, ori, ci, cf, step)) {
                Gate gateObj(x, y, ori, ci, cf, step);
                outLevel.board->addGate(gateObj);
            }
        }
    }

    std::fclose(file);
    return true;
}

void FileParser::freeLevel(ParsedLevel& level) {
    if (level.geometries != nullptr) {
        for (int i = 0; i < level.blockCount; ++i) {
            delete[] level.geometries[i];
        }
        delete[] level.geometries;
    }

    delete[] level.blocks;
    delete level.board;

    resetParsedLevel(level);
}
