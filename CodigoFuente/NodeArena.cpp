#include "NodeArena.h"

NodeArena::NodeArena()
    : nodes(nullptr), count(0), capacity(0) {}

NodeArena::~NodeArena() {
    clear();
    delete[] nodes;
}

void NodeArena::ensureCapacity(int minCapacity) {
    if (capacity >= minCapacity) return;

    int newCapacity = (capacity == 0) ? 256 : capacity;
    while (newCapacity < minCapacity) {
        newCapacity *= 2;
    }

    AStarNode** newNodes = new AStarNode*[newCapacity];
    for (int i = 0; i < count; ++i) {
        newNodes[i] = nodes[i];
    }

    delete[] nodes;
    nodes = newNodes;
    capacity = newCapacity;
}

AStarNode* NodeArena::create(const GameState& state,
                             AStarNode* parent,
                             const GameState::Move& move,
                             int g,
                             int h) {
    ensureCapacity(count + 1);

    AStarNode* node = new AStarNode();
    node->state = new GameState(state);
    node->parent = parent;
    node->moveFromParent = move;
    node->g = g;
    node->h = h;
    node->f = g + h;

    nodes[count++] = node;
    return node;
}

void NodeArena::clear() {
    for (int i = 0; i < count; ++i) {
        delete nodes[i]->state;
        delete nodes[i];
    }
    count = 0;
}

int NodeArena::size() const {
    return count;
}
