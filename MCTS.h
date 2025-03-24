// MCTS.h
#ifndef MCTS_H
#define MCTS_H

#include "Node.h"
#include <chrono>

class MCTS {
public:
    MCTS(State initialState, int player, double timeLimit);
    ~MCTS(); // Destructor para liberar memoria.
    std::pair<int, int> findBestMove();
    void reuseTree(const std::pair<int, int>& move); // Reutiliza el árbol.

private:
    Node* root;
    int player;
    double timeLimit;

    void deleteTree(Node* node);
};

#endif