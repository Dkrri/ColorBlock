#include <iostream>
#include "Exit.h"

int main() {
    Exit ex(0, 1, 'a', 'V', 2, 4, 1);
    if (ex.getX() != 0 || ex.getY() != 1) return 1;
    if (ex.getColor() != 'a' || ex.getOrientation() != 'V') return 1;
    if (ex.getLengthAtStep(0) != 2) return 1;
    if (ex.getLengthAtStep(1) != 3) return 1;
    if (ex.getLengthAtStep(2) != 4) return 1;
    if (ex.getLengthAtStep(3) != 3) return 1;

    std::cout << "test_exit OK\n";
    return 0;
}
