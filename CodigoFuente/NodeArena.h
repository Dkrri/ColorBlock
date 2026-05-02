#pragma once

#include "AStarNode.h"

class NodeArena {
private:
    AStarNode** nodes;
    int count;
    int capacity;

    void ensureCapacity(int minCapacity);

public:
    NodeArena();
    ~NodeArena();

    AStarNode* create(const GameState& state,
                      AStarNode* parent,
                      const GameState::Move& move,
                      int g,
                      int h);

    void clear();
    int size() const;
};
