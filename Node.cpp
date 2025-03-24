#include "Node.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>

// Constructor: Inicializa un nodo con un estado del juego, un movimiento y un nodo padre.
Node::Node(Node* parent, std::pair<int, int> move, State state)
    : parent(parent), move(move), state(state), visits(0), value(0) {}

// Selecciona el mejor hijo del nodo actual utilizando el criterio UCB1.
Node* Node::selectChild() {
    Node* selected = nullptr;
    double bestValue = -std::numeric_limits<double>::infinity();

    for (auto& child : children) {
        // Verificar si el hijo tiene una jugada ganadora.
        auto winningMove = child->state.findWinningOrBlockingMove(child->state.currentPlayer);
        if (winningMove.first != -1 && winningMove.second != -1) {
            return child.get(); // Seleccionar el hijo si tiene una jugada ganadora.
        }

        // Verificar si el hijo tiene una jugada para bloquear al oponente.
        auto blockingMove = child->state.findWinningOrBlockingMove(3 - (child->state.currentPlayer));
        if (blockingMove.first != -1 && blockingMove.second != -1) {
            return child.get(); // Seleccionar el hijo si tiene una jugada bloqueadora.
        }

        // Calcular el valor UCB1 para el hijo.
        double uctValue = child->value / child->visits + std::sqrt(2 * std::log(visits) / child->visits);
        if (uctValue > bestValue) {
            selected = child.get(); // Seleccionar el hijo con el mejor valor UCB1.
            bestValue = uctValue;
        }
    }

    return selected; // Devolver el hijo seleccionado.
}

// Expande el nodo actual generando nuevos hijos a partir de movimientos posibles.
void Node::expand() {
    auto availableMoves = state.getAvailableMoves();

    // Verificar si hay una jugada ganadora para el jugador actual.
    auto winningMove = state.findWinningOrBlockingMove(state.currentPlayer);
    if (winningMove.first != -1 && winningMove.second != -1) {
        State newState = state;
        newState.makeMove(winningMove.first, winningMove.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, winningMove, newState));
        return; // Priorizar la jugada ganadora.
    }

    // Verificar si hay una jugada para bloquear al oponente.
    auto blockingMove = state.findWinningOrBlockingMove(3 - state.currentPlayer);
    if (blockingMove.first != -1 && blockingMove.second != -1) {
        State newState = state;
        newState.makeMove(blockingMove.first, blockingMove.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, blockingMove, newState));
        return; // Priorizar la jugada bloqueadora.
    }

    // Evaluar y ordenar los movimientos disponibles según su puntuación.
    std::vector<std::pair<std::pair<int, int>, double>> scoredMoves;
    for (const auto& move : availableMoves) {
        double score = state.evaluateMove(move.first, move.second);
        scoredMoves.push_back({move, score});
    }

    // Ordenar los movimientos por puntuación (de mayor a menor).
    sort(scoredMoves.begin(), scoredMoves.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Expandir los movimientos con mayor puntuación.
    for (const auto& [move, score] : scoredMoves) {
        State newState = state;
        newState.makeMove(move.first, move.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, move, newState));
    }
}

// Simula una partida aleatoria desde el estado actual hasta un estado terminal.
double Node::simulate() {
    State simState = state;

    while (!simState.isTerminal()) {
        // Verificar si el jugador actual puede ganar.
        auto winningMove = simState.findWinningOrBlockingMove(simState.currentPlayer);
        if (winningMove.first != -1 && winningMove.second != -1) {
            simState.makeMove(winningMove.first, winningMove.second, simState.currentPlayer);
            continue; // Realizar la jugada ganadora.
        }

        // Verificar si el oponente puede ganar.
        auto blockingMove = simState.findWinningOrBlockingMove(3 - simState.currentPlayer);
        if (blockingMove.first != -1 && blockingMove.second != -1) {
            simState.makeMove(blockingMove.first, blockingMove.second, simState.currentPlayer);
            continue; // Bloquear la jugada del oponente.
        }

        // Si no hay jugadas críticas, seleccionar un movimiento aleatorio ponderado.
        auto moves = simState.getAvailableMoves();
        if (!moves.empty()) {
            std::vector<std::pair<std::pair<int, int>, double>> scoredMoves;
            for (const auto& move : moves) {
                double score = simState.evaluateMove(move.first, move.second);
                scoredMoves.push_back({move, score});
            }

            double totalScore = 0;
            for (const auto& [move, score] : scoredMoves) {
                totalScore += score;
            }

            double randomVal = static_cast<double>(rand()) / RAND_MAX * totalScore;
            double cumulativeScore = 0;

            for (const auto& [move, score] : scoredMoves) {
                cumulativeScore += score;
                if (cumulativeScore >= randomVal) {
                    simState.makeMove(move.first, move.second, simState.currentPlayer);
                    break;
                }
            }
        } else {
            break; // No hay movimientos disponibles.
        }
    }

    // Determinar el resultado de la simulación.
    if (simState.checkWin(state.currentPlayer)) {
        return 1.0; // Victoria del jugador actual.
    } else if (simState.checkWin(3 - state.currentPlayer)) {
        return 0.0; // Derrota del jugador actual.
    } else {
        return 0.5; // Empate.
    }
}

// Retropropaga el resultado de la simulación hacia arriba en el árbol.
void Node::backpropagate(double value) {
    Node* current = this;
    while (current != nullptr) {
        current->visits++; // Incrementar el número de visitas al nodo.
        current->value += value; // Actualizar el valor acumulado del nodo.
        current = current->parent; // Moverse al nodo padre.
    }
}