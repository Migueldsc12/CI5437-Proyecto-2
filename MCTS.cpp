// MCTS.cpp
#include "MCTS.h"
#include <chrono>

MCTS::MCTS(State initialState, int player, double timeLimit)
    : root(new Node(nullptr, {-1, -1}, initialState)), player(player), timeLimit(timeLimit) {}

std::pair<int, int> MCTS::findBestMove() {
    auto startTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {        
        Node* node = root;
        
        // Selección
        while (!node->children.empty()) {
            node = node->selectChild();
        }

        // Expansión
        if (!node->state.isTerminal()) {
            node->expand();
            node = node->children[0].get(); // Selecciona el primer hijo para simular
        }

        // Simulación
        double result = node->simulate();

        // Retropropagación
        node->backpropagate(result);
    }
    
    // Selecciona el mejor movimiento basado en las visitas
    Node* bestChild = nullptr;
    int mostVisits = -1;

    for (auto& child : root->children) {
        if (child->visits > mostVisits) {
            bestChild = child.get();
            mostVisits = child->visits;
        }
    }

    return bestChild ? bestChild->move : std::make_pair(-1, -1);
}