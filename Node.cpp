// Node.cpp
#include "Node.h"
#include <cmath>
#include <algorithm>

Node::Node(Node* parent, std::pair<int, int> move, State state)
    : parent(parent), move(move), state(state), visits(0), value(0) {}

Node* Node::selectChild() {
    Node* selected = nullptr;
    double bestValue = -std::numeric_limits<double>::infinity();

    for (auto& child : children) {
        double uctValue = child->value / child->visits + std::sqrt(2 * std::log(visits) / child->visits);
        if (uctValue > bestValue) {
            selected = child.get();
            bestValue = uctValue;
        }
    }

    return selected;
}

void Node::expand() {
    auto availableMoves = state.getAvailableMoves();
    for (const auto& move : availableMoves) {
        State newState = state;
        newState.makeMove(move.first, move.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, move, newState));
    }
}

double Node::simulate() {
    State simState = state;
    while (!simState.isTerminal()) {
        auto moves = simState.getAvailableMoves();
        auto randomMove = moves[rand() % moves.size()];
        simState.makeMove(randomMove.first, randomMove.second, simState.currentPlayer);
    }

    if (simState.checkWin(state.currentPlayer)) {
        return 1.0; // Victoria
    } else if (simState.checkWin(3 - state.currentPlayer)) {
        return 0.0; // Derrota
    } else {
        return 0.5; // Empate
    }
}

void Node::backpropagate(double value) {
    Node* current = this;
    while (current != nullptr) {
        current->visits++;
        current->value += value;
        current = current->parent;
    }
}