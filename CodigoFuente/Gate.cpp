#include "Gate.h"

Gate::Gate()
    : x(0),
      y(0),
      orientacion(' '),
      colorInicial(' '),
      colorFinal(' '),
      pasosCambio(0) {}

// Inicializa la compuerta con sus colores y paso de cambio.
Gate::Gate(int _x, int _y, char _ori, char _ci, char _cf, int _paso)
    : x(_x),
      y(_y),
      orientacion(_ori),
      colorInicial(_ci),
      colorFinal(_cf),
      pasosCambio(_paso) {}

int Gate::getX() const {
    return x;
}

int Gate::getY() const {
    return y;
}

char Gate::getOrientation() const {
    return orientacion;
}

char Gate::getInitialColor() const {
    return colorInicial;
}

char Gate::getFinalColor() const {
    return colorFinal;
}

int Gate::getStepChange() const {
    return pasosCambio;
}

void Gate::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

char Gate::getColorAtStep(int step) const {
    if (pasosCambio <= 0 || colorFinal < colorInicial) {
        return colorInicial;
    }
    int changes = step / pasosCambio;
    int range = (colorFinal - colorInicial) + 1;
    return (char)(colorInicial + (changes % range));
}

bool Gate::isFinalStateAtStep(int step) const {
    return getColorAtStep(step) == colorFinal;
}
