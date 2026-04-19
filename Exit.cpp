#include "Exit.h"

// Configura la salida con su posicion, color y reglas de longitud.
Exit::Exit(int _x, int _y, char _color, char _ori, int _li, int _lf, int _step)
    : x(_x),
      y(_y),
      color(_color),
      orientation(_ori),
      initialLength(_li),
      finalLength(_lf),
      stepChange(_step) {}

int Exit::getX() const {
    return x;
}

int Exit::getY() const {
    return y;
}

char Exit::getColor() const {
    return color;
}

char Exit::getOrientation() const {
    return orientation;
}

int Exit::getInitialLength() const {
    return initialLength;
}

int Exit::getFinalLength() const {
    return finalLength;
}

int Exit::getStepChange() const {
    return stepChange;
}

int Exit::getLengthAtStep(int step) const {
    if (step <= 0) {
        return initialLength;
    }

    int length = initialLength + (step * stepChange);
    if (stepChange >= 0 && length > finalLength) {
        return finalLength;
    }
    if (stepChange < 0 && length < finalLength) {
        return finalLength;
    }
    return length;
}

bool Exit::reachesFinalLengthAtStep(int step) const {
    return getLengthAtStep(step) == finalLength;
}
