#include <iostream>
#include "BestCostTable.h"

int main() {
    BestCostTable table;
    int value = -1;

    if (!table.setIfBetter(10ul, 5)) return 1;
    if (!table.tryGet(10ul, value) || value != 5) return 1;
    if (table.setIfBetter(10ul, 8)) return 1;
    if (!table.setIfBetter(10ul, 3)) return 1;
    if (!table.tryGet(10ul, value) || value != 3) return 1;

    std::cout << "test_bestcosttable OK\n";
    return 0;
}
