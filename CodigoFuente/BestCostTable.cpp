#include "BestCostTable.h"

BestCostTable::BestCostTable()
    : keys(nullptr), values(nullptr), used(nullptr), capacity(0), count(0) {}

BestCostTable::~BestCostTable() {
    delete[] keys;
    delete[] values;
    delete[] used;
}

void BestCostTable::ensureCapacity(int minCapacity) {
    if (capacity >= minCapacity) return;

    int newCapacity = 1;
    while (newCapacity < minCapacity) {
        newCapacity <<= 1;
    }

    unsigned long* newKeys = new unsigned long[newCapacity];
    int* newValues = new int[newCapacity];
    unsigned char* newUsed = new unsigned char[newCapacity];

    for (int i = 0; i < newCapacity; ++i) {
        newKeys[i] = 0;
        newValues[i] = 0;
        newUsed[i] = 0;
    }

    if (keys != nullptr) {
        for (int i = 0; i < capacity; ++i) {
            if (!used[i]) continue;

            unsigned long k = keys[i];
            int v = values[i];
            int index = (int)(k & (unsigned long)(newCapacity - 1));
            while (newUsed[index]) {
                index = (index + 1) & (newCapacity - 1);
            }

            newUsed[index] = 1;
            newKeys[index] = k;
            newValues[index] = v;
        }

        delete[] keys;
        delete[] values;
        delete[] used;
    }

    keys = newKeys;
    values = newValues;
    used = newUsed;
    capacity = newCapacity;
}

bool BestCostTable::tryGet(unsigned long key, int& outValue) const {
    if (capacity == 0) return false;

    int index = (int)(key & (unsigned long)(capacity - 1));
    int start = index;

    while (used[index]) {
        if (keys[index] == key) {
            outValue = values[index];
            return true;
        }
        index = (index + 1) & (capacity - 1);
        if (index == start) break;
    }

    return false;
}

bool BestCostTable::setIfBetter(unsigned long key, int value) {
    if (capacity == 0) ensureCapacity(256);
    if ((count + 1) * 10 > capacity * 7) ensureCapacity(capacity * 2);

    int index = (int)(key & (unsigned long)(capacity - 1));
    while (used[index]) {
        if (keys[index] == key) {
            if (value < values[index]) {
                values[index] = value;
                return true;
            }
            return false;
        }
        index = (index + 1) & (capacity - 1);
    }

    used[index] = 1;
    keys[index] = key;
    values[index] = value;
    ++count;
    return true;
}

int BestCostTable::size() const {
    return count;
}
