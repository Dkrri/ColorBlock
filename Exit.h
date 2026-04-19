#pragma once

class Exit {
private:
    int x, y;
    char color;
    char orientation;
    int initialLength;
    int finalLength;
    int stepChange;

public:
    // Constructor principal de una salida con crecimiento por pasos.
    Exit(int _x, int _y, char _color, char _ori, int _li, int _lf, int _step);

    // Getters para consultar estado de la salida.
    int getX() const;
    int getY() const;
    char getColor() const;
    char getOrientation() const;
    int getInitialLength() const;
    int getFinalLength() const;
    int getStepChange() const;

    // Devuelve largo actual segun el paso de simulacion.
    int getLengthAtStep(int step) const;

    // Indica si la salida ya alcanzo su largo objetivo.
    bool reachesFinalLengthAtStep(int step) const;
};