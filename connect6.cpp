#include <iostream>
#include <vector>
#include "connect6.h"
using namespace std;

State::State() : size(19), current(vector<vector<int>>(size, vector<int>(size, 0))), currentPlayer(1) {}

bool State::checkWin(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == player) {
                for (const auto& [dx, dy] : directions) {
                    int count = 1;
                    for (int k = 1; k < 6; k++) {
                        int px = i + k * dx;
                        int py = j + k * dy;
                        if (px >= 0 && px < size && py >= 0 && py < size && current[px][py] == player) {
                            count++;
                        } else {
                            break;
                        }
                    }
                    if (count == 6) {
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

std::vector<std::pair<int, int>> State::getAvailableMoves() {
    std::vector<std::pair<int, int>> moves;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) {
                moves.push_back({i, j});
            }
        }
    }
    return moves;
}

bool State::isTerminal() {
    return checkWin(1) || checkWin(2) || getAvailableMoves().empty();
}

std::pair<int, int> State::findWinningOrBlockingMove(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == player) {
                for (const auto& [dx, dy] : directions) {
                    int count = 1;
                    for (int k = 1; k < 6; k++) {
                        int px = i + k * dx;
                        int py = j + k * dy;
                        if (px >= 0 && px < size && py >= 0 && py < size && current[px][py] == player) {
                            count++;
                        } else {
                            break;
                        }
                    }
                    if (count >= 4 && count < 6) { // Si el jugador tiene 4 o 5 fichas en linea.
                        int px = i + count * dx;
                        int py = j + count * dy;
                        if (px >= 0 && px < size && py >= 0 && py < size && current[px][py] == 0) {
                            return {px, py};
                        }
                    }
                }
            }
        }
    }
    return {-1, -1};
}

