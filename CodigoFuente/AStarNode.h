#pragma once

#include "GameState.h"

struct AStarNode {
    GameState* state;
    AStarNode* parent;
    GameState::Move moveFromParent;
    int g;
    int h;
    int f;

    AStarNode()
        : state(nullptr), parent(nullptr), moveFromParent({-1, 0, 0}), g(0), h(0), f(0) {}
};
