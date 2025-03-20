// Node.h
#ifndef NODE_H
#define NODE_H

#include "connect6.h"
#include <vector>
#include <memory>

class Node {
public:
    Node(Node* parent, std::pair<int, int> move, State state);
    Node* selectChild();
    void expand();
    double simulate();
    void backpropagate(double value);

    Node* parent;
    std::pair<int, int> move; // Movimiento que llevó a este estado
    State state; // Estado del juego en este nodo
    std::vector<std::unique_ptr<Node>> children;
    int visits;
    double value;
};

#endif