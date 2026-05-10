#pragma once

class Gate {
private:
    int x, y;
    char orientacion;
    int longitudInicial;
    char colorInicial;
    char colorFinal;
    int pasosCambio;

public:
    // Constructor por defecto.
    Gate();

    // Constructor de una compuerta con cambio de color por tiempo.
    Gate(int _x, int _y, char _ori, char _ci, char _cf, int _paso);

    // Constructor de compuerta con longitud explicita.
    Gate(int _x, int _y, char _ori, int _li, char _ci, char _cf, int _paso);

    // Getters basicos para posicion y configuracion.
    int getX() const;
    int getY() const;
    char getOrientation() const;
    int getInitialLength() const;
    char getInitialColor() const;
    char getFinalColor() const;
    int getStepChange() const;

    // Cambia la posicion de la compuerta en el mapa.
    void setPosition(int newX, int newY);

    // Determina color visible segun el paso actual.
    char getColorAtStep(int step) const;

    // Indica si el estado final ya esta activo.
    bool isFinalStateAtStep(int step) const;

    // Indica si la compuerta ocupa una celda en su segmento (segun orientacion y longitud).
    bool occupiesCell(int row, int col) const;
};