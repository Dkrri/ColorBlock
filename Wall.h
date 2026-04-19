#pragma once

class Wall {
private:
    int x;
    int y;
    bool exitWall;
    bool gateWall;

public:
    // Crea una pared en una coordenada, opcionalmente marcada como salida o gate.
    Wall(int _x, int _y, bool _isExit = false, bool _isGate = false);

    // Getters basicos de posicion y tipo.
    int getX() const;
    int getY() const;
    bool isExitWall() const;
    bool isGateWall() const;

    // Marca la pared como salida normal.
    void setExitWall(bool value);

    // Marca la pared como pared asociada a gate.
    void setGateWall(bool value);

    // Actualiza coordenadas de la pared.
    void setPosition(int newX, int newY);

    // Verifica si la pared ocupa una celda exacta.
    bool occupiesCell(int cellX, int cellY) const;
};
