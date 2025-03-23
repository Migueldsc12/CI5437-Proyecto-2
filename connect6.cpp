#include <iostream>
#include <vector>
#include <cmath>
#include <limits.h>
#include "connect6.h"

using namespace std;

State::State() : size(19), current(vector<vector<int>>(size, vector<int>(size, 0))), currentPlayer(1) {}

int State::countInDirection(int x, int y, int dx, int dy, int player, int maxGaps){
    int count = 0;
    int gaps = 0;

    for (int k = 1; k < 6; k++) {
        int nx = x + k * dx;
        int ny = y + k * dy;

        if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
            if (current[nx][ny] == player) {
                count++;
            } else if (current[nx][ny] == 0 && gaps < maxGaps) {
                gaps++;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    return count;
}

int State::countInBothDirections(int x, int y, int dx, int dy, int player, int maxGaps){
    int count = 1;
    count += countInDirection(x, y, dx, dy, player, maxGaps); // Direccion positiva
    count += countInDirection(x, y, -dx, -dy, player, maxGaps); // Direccion negativa
    return count;
}

bool State::checkWin(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == player) {
                for (const auto& [dx, dy] : directions) {
                    if (countInBothDirections(i, j, dx, dy, player, 0) >= 6) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool State::makeMove(int x, int y, int player) {
    if (x >= 0 && x < size && y >= 0 && y < size && current[x][y] == 0) {
        current[x][y] = player;
        return true;
    }
    return false;
}

vector<pair<int, int>> State::getAvailableMoves(){
    vector<pair<int, int>> moves;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) {
                moves.push_back({i, j});
            }
        }
    }
    return moves;
}

bool State::isTerminal(){
    return checkWin(1) || checkWin(2) || getAvailableMoves().empty();
}

bool State::checkWinAfterMove(int x, int y, int player) {
    State tempState = *this;
    tempState.makeMove(x, y, player);
    return tempState.checkWin(player);
}

double State::evaluateMove(int x, int y) {
    double score = 0.0;
    vector<pair<int, int>> directions = {{-1, -1}, {-1, 0}, {-1, 1},
                                        {0, -1},          {0, 1},
                                        {1, -1},  {1, 0}, {1, 1}};

    //---- Caso: Piezas adyacentes del mismo tipo
    int adjacentPieces = 0;
    for (const auto& [dx, dy] : directions) {
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < size && ny >= 0 && ny < size && current[nx][ny] == currentPlayer) {
            adjacentPieces++;
        }
    }
    score += adjacentPieces * 5;

    //---- Caso: Si provoca que haya 5 fichas en linea (incluyendo gaps)
    int lineCompletionScore = 0;
    for (const auto& [dx, dy] : directions) {
        int count = countInBothDirections(x, y, dx, dy, currentPlayer, 1); // Permitir 1 gap
        if (count >= 4) { //  4 fichas en linea con un espacio vacio
            lineCompletionScore += 1000;
        } else if (count >= 3) { // 3 fichas en linea con un espacio vacio
            lineCompletionScore += 500; // Prioridad alta
        }
    }
    score += lineCompletionScore;

    //---- Caso: Si bloquea una linea de 5 fichas del oponente (incluyendo gaps)
    int opponent = 3 - currentPlayer;
    int blockingScore = 0;

    for (const auto& [dx, dy] : directions) {
        int count = countInBothDirections(x, y, dx, dy, opponent, 1);
        if (count >= 4) {
            blockingScore += 100;
        } else if (count >= 3) {
            blockingScore += 50;
        }
    }
    score += blockingScore;

    //---- Caso: Extensionn de lineas existentes
    int lineExtensionScore = 0;
    for (const auto& [dx, dy] : directions) {
        int lineLength = countInBothDirections(x, y, dx, dy, currentPlayer, 1);
        if (lineLength >= 2) {
            lineExtensionScore += lineLength * 5;
        }
    }
    score += lineExtensionScore;

    //---- Caso: Proximidad al centro del tablero
    int center = size / 2;
    double distanceToCenter = sqrt((x - center) * (x - center) + (y - center) * (y - center));
    score += (size - distanceToCenter) * 2;

    return score;
}

pair<int, int> State::findWinningOrBlockingMove(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) {
                for (const auto& [dx, dy] : directions) {
                    if (countInBothDirections(i, j, dx, dy, player, 1) >= 5) {
                        return {i, j};
                    }
                }
            }
        }
    }

    return {-1, -1};
}