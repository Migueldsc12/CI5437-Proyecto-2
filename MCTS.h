// MCTS.h
#ifndef MCTS_H
#define MCTS_H

#include "Node.h"
#include <chrono>

class MCTS {
public:
    MCTS(State initialState, int player, double timeLimit);
    std::pair<int, int> findBestMove();

private:
    Node* root;
    int player;
    double timeLimit;
};

#endif