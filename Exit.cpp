#include "Exit.h"

Exit::Exit()
    : x(0),
      y(0),
      color(' '),
      orientation(' '),
      initialLength(0),
      finalLength(0),
      stepChange(0) {}

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
    if (stepChange <= 0 || initialLength == finalLength) {
        return initialLength;
    }

    int changes = step / stepChange;
    int diff = initialLength > finalLength ? (initialLength - finalLength) : (finalLength - initialLength);
    int cycle = 2 * diff;
    int mod = changes % cycle;
    int offset = (mod <= diff) ? mod : (cycle - mod);

    if (initialLength >= finalLength) {
        return initialLength - offset;
    } else {
        return initialLength + offset;
    }
}

bool Exit::reachesFinalLengthAtStep(int step) const {
    return getLengthAtStep(step) == finalLength;
}
