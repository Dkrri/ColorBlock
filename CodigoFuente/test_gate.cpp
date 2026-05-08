#include <iostream>
#include "Gate.h"

int main() {
    Gate gate(2, 3, 'H', 'a', 'c', 1);
    if (gate.getX() != 2 || gate.getY() != 3) return 1;
    if (gate.getOrientation() != 'H') return 1;
    if (gate.getColorAtStep(0) != 'a') return 1;
    if (gate.getColorAtStep(1) != 'b') return 1;
    if (gate.getColorAtStep(2) != 'c') return 1;
    if (gate.getColorAtStep(3) != 'a') return 1;

    std::cout << "test_gate OK\n";
    return 0;
}
