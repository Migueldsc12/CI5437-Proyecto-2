#include <iostream>
#include <vector>
#include "connect6.h"
#include <math.h>
#include <limits.h>

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

vector<pair<int, int>> State::getAvailableMoves() {
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

bool State::checkWinAfterMove(int x, int y, int player) {
    // Hacer una copia temporal del estado y aplicar el movimiento
    State tempState = *this;
    tempState.makeMove(x, y, player);

    // Verificar si el movimiento resulta en una victoria
    return tempState.checkWin(player);
}

double State::evaluateMove(int x, int y) {
    // Puntuación base
    double score = 0.0;

    vector<pair<int, int>> directions = {{-1, -1}, {-1, 0}, {-1, 1},
                                        {0, -1},          {0, 1},
                                        {1, -1},  {1, 0}, {1, 1}};
    
                                        //---- Caso: Piezas adyacentes del mismo tipo
    int adjacentPieces = 0;
    for (const auto& [dx, dy] : directions) {
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < size && ny >= 0 && ny < size && current[nx][ny] == currentPlayer ) {
            adjacentPieces++;
        }
    }
    score += adjacentPieces * 5;
    


    //---- Caso: Si bloquea una linea de 5 fichas del oponente
    int opponent = 3 - currentPlayer;
    int blockingScore = 0;

    for (const auto& [dx, dy] : directions) {
        int count = 0;
        int gaps = 0;

        // Verificar en la direccion (dx, dy)
        for (int k = 1; k < 4; k++) {
            int nx = x + k * dx;
            int ny = y + k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == opponent) {
                    count++;
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir un espacio vacío
                } else {
                    break; // Romper si hay una ficha del jugador o demasiados espacios
                }
            } else {
                break; // Romper si se sale del tablero
            }
        }

        // Verificar en la direccionn (-dx, -dy)
        for (int k = 1; k < 6; k++) {
            int nx = x - k * dx;
            int ny = y - k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == opponent) {
                    count++;
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir espacio vacio
                } else {
                    break; // Romper si hay una ficha del jugador o demasiados espacios
                }
            } else {
                break; // Romper si se sale del tablero
            }
        }

        if (count >= 4) { // Si el oponente tiene 4 o mas fichas en linea
            blockingScore += 1000;
        } else if (count >= 3) { // Si el oponente tiene 3 fichas en linea
            blockingScore += 50;
        }
    }
    score += blockingScore;

    
    //---- Caso: Extension de lineas existentes
    int lineExtensionScore = 0;
    for (const auto& [dx, dy] : directions) {
        int lineLength = 1;
        int gaps = 0;

        // Verificar en la direccion (dx, dy)
        for (int k = 1; k < 6; k++) {
            int nx = x + k * dx;
            int ny = y + k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == currentPlayer) {
                    lineLength++;
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir espacio vacio
                } else {
                    break; //si hay una ficha del oponente o demasiados espacios
                }
            } else {
                break; //se sale del tablero
            }
        }

        // Verificar en la direccion (-dx, -dy)
        for (int k = 1; k < 6; k++) {
            int nx = x - k * dx;
            int ny = y - k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == currentPlayer) {
                    lineLength++;
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir espacio vacio
                } else {
                    break; //si hay una ficha del oponente o demasiados espacios
                }
            } else {
                break; //si se sale del tablero
            }
        }

        // Asignar puntuacion segun la longitud de la linea
        if (lineLength >= 2) {
            lineExtensionScore += lineLength * 10;
        }
    }
    score += lineExtensionScore;

    //---- Caso: Proximidad al centro del tablero
    int center = size / 2;
    double distanceToCenter = sqrt((x - center) * (x - center) + (y - center) * (y - center));
    score += (size - distanceToCenter) *2; 

    return score;
}


pair<int, int> State::findWinningOrBlockingMove(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) {
                for (const auto& [dx, dy] : directions) {
                    int count = 0;
                    int gaps = 0;
                    
                    // (dx, dy)
                    for (int k = 1; k < 6; k++) {
                        int px = i + k * dx;
                        int py = j + k * dy;

                        if (px >= 0 && px < size && py >= 0 && py < size) {
                            if (current[px][py] == player) {
                                count++;
                            } else if (current[px][py] == 0 && gaps < 1) {
                                gaps++; // Permitir un espacio vacio
                            } else {
                                break; // Romper si hay una ficha del oponente o demasiados espacios
                            }
                        } else {
                            break; // Romper si se sale del tablero
                        }
                    }

                    // (-dx, -dy)
                    for (int k = 1; k < 6; k++) {
                        int px = i - k * dx;
                        int py = j - k * dy;

                        if (px >= 0 && px < size && py >= 0 && py < size) {
                            if (current[px][py] == player) {
                                count++;
                            } else if (current[px][py] == 0 && gaps < 1) {
                                gaps++; // Permitir espacio vacio
                            } else {
                                break; // Romper si hay una ficha del oponente o demasiados espacios
                            }
                        } else {
                            break; // Romper si se sale del tablero
                        }
                    }
                    if (count >= 5) {
                        return {i, j};
                    }
                }
            }
        }
    }

    return {-1, -1};
}

