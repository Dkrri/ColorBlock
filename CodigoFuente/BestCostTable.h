#pragma once

class BestCostTable {
private:
    unsigned long* keys;
    int* values;
    unsigned char* used;
    int capacity;
    int count;

    void ensureCapacity(int minCapacity);

public:
    BestCostTable();
    ~BestCostTable();

    bool tryGet(unsigned long key, int& outValue) const;
    bool setIfBetter(unsigned long key, int value);

    int size() const;
};
