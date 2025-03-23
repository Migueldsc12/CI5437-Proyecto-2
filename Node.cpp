// Node.cpp
#include "Node.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>

Node::Node(Node* parent, std::pair<int, int> move, State state)
    : parent(parent), move(move), state(state), visits(0), value(0) {}

Node* Node::selectChild() {
    Node* selected = nullptr;
    double bestValue = -std::numeric_limits<double>::infinity();

    for(auto& child : children){
        auto winningMove = child -> state.findWinningOrBlockingMove(child->state.currentPlayer);
        if(winningMove.first != -1 && winningMove.second != -1){
            return child.get();
        }

        auto blockingMove = child->state.findWinningOrBlockingMove(3 - (child->state.currentPlayer));
        if(blockingMove.first != -1 && blockingMove.second != -1){
            return child.get();
        }


        double uctValue = child->value / child->visits + std::sqrt(2 * std::log(visits) / child->visits);
        if (uctValue > bestValue) {
            selected = child.get();
            bestValue = uctValue;
        }
    }return selected;
}


void Node::expand() {
    auto availableMoves = state.getAvailableMoves();

    // Verificar si hay una jugada para ganar
    auto winningMove = state.findWinningOrBlockingMove(state.currentPlayer);
    if(winningMove.first != -1 && winningMove.second!= -1){
        State newState = state;
        newState.makeMove(winningMove.first, winningMove.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, winningMove, newState));
        return; // Prioriza ganar
    }
    
    // Verificar si hay una jugada para bloquear al jugador
    auto blockingMove = state.findWinningOrBlockingMove(3-state.currentPlayer);
    if(blockingMove.first != -1 && blockingMove.second != -1 ){
        State newState = state;
        newState.makeMove(blockingMove.first, blockingMove.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, blockingMove, newState));
        return; // Prioritiza bloqueo
    }

    // (movimiento, puntuacion)
    std::vector<std::pair<std::pair<int, int>, double>> scoredMoves;
    for (const auto& move : availableMoves) {
        double score = state.evaluateMove(move.first,move.second);
        scoredMoves.push_back({move, score});
    }

    // Ordenar los movimientos por puntuacion (de mayor a menor)
    sort(scoredMoves.begin(), scoredMoves.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Expandir los movimientos con mayor puntuacion
    for (const auto& [move, score] : scoredMoves) {
        State newState = state;
        newState.makeMove(move.first, move.second, state.currentPlayer);
        children.push_back(std::make_unique<Node>(this, move, newState));
    }
}  

double Node::simulate() {
    State simState = state;
    
    while (!simState.isTerminal()) {
        // Verificar si el agente puede ganar
        auto winningMove = simState.findWinningOrBlockingMove(simState.currentPlayer);
        if(winningMove.first != -1 && winningMove.second != -1 ){
            simState.makeMove(winningMove.first, winningMove.second, simState.currentPlayer);
            continue;
        }

        // Verificar si el jugador puede ganar
        auto blockingMove = simState.findWinningOrBlockingMove(3 - simState.currentPlayer);
        if( blockingMove.first != -1 && blockingMove.second != -1){
            simState.makeMove(blockingMove.first, blockingMove.second, simState.currentPlayer);
            continue;
        } 
        
        auto moves = simState.getAvailableMoves();
        if (!moves.empty()) {
            std::vector<std::pair<std::pair<int,int>, double >> scoredMoves;
            for(const auto& move:moves){
                double score = simState.evaluateMove(move.first, move.second);
                scoredMoves.push_back({move,score});
            }
            double totalScore= 0;
            for(const auto& [move,score]: scoredMoves){
                totalScore += score;
            }

            double randomVal = static_cast<double>(rand()) / RAND_MAX * totalScore;
            double cumlativeScore = 0;
            
            for(const auto& [move,score]: scoredMoves){
                cumlativeScore += score;
                if(cumlativeScore >= randomVal){
                    simState.makeMove(move.first,move.second,simState.currentPlayer);
                    break;
                }
            }
        }else {
        break;
        }
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