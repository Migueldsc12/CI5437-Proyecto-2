#include "MCTS.h"
#include <chrono>
#include <omp.h> // Para paralelización con OpenMP

// Constructor: Inicializa el MCTS con un estado inicial, un jugador y un límite de tiempo.
MCTS::MCTS(State initialState, int player, double timeLimit)
    : root(new Node(nullptr, {-1, -1}, initialState)), player(player), timeLimit(timeLimit) {}

// Destructor: Libera la memoria del árbol.
MCTS::~MCTS() {
    deleteTree(root);
}

// Libera la memoria del árbol recursivamente.
void MCTS::deleteTree(Node* node) {
    if (!node) return;
    for (auto& child : node->children) {
        deleteTree(child.get());
    }
}

// Encuentra el mejor movimiento utilizando MCTS.
std::pair<int, int> MCTS::findBestMove() {
    auto startTime = std::chrono::steady_clock::now();

    #pragma omp parallel
    {
        #pragma omp single
        {
            while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {
                #pragma omp task
                {
                    Node* node = root;

                    // Selección: Recorre el árbol hasta llegar a un nodo hoja.
                    while (!node->children.empty()) {
                        node = node->selectChild();
                    }

                    // Expansión: Expande el nodo si no es terminal.
                    if (!node->state.isTerminal()) {
                        node->expand();
                        node = node->children[0].get(); // Selecciona el primer hijo para simular.
                    }

                    // Simulación: Realiza una simulación desde el nodo expandido.
                    double result = node->simulate();

                    // Retropropagación: Retropropaga el resultado de la simulación.
                    node->backpropagate(result);
                }
            }
        }
    }

    // Selecciona el mejor movimiento basado en las visitas.
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

// Reutiliza el árbol para el siguiente turno.
void MCTS::reuseTree(const std::pair<int, int>& move) {
    for (auto& child : root->children) {
        if (child->move == move) {
            // El hijo seleccionado se convierte en la nueva raíz.
            root = child.release();
            root->parent = nullptr;
            return;
        }
    }

    // Si no se encuentra el movimiento, reiniciar el árbol.
    deleteTree(root);
    root = new Node(nullptr, {-1, -1}, root->state);
}