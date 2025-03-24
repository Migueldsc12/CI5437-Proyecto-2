#include <iostream>
#include <vector>
#include "connect6.h"
#include <cmath>
#include <limits.h>

using namespace std;

// Constructor: Inicializa el estado del juego con un tablero vacío de 19x19.
State::State() : size(19), current(vector<vector<int>>(size, vector<int>(size, 0))), currentPlayer(1) {}

// Verifica si un jugador ha ganado el juego.
bool State::checkWin(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}}; // Direcciones posibles para formar una línea.

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == player) {
                for (const auto& [dx, dy] : directions) {
                    int count = 1; // Contador de fichas consecutivas.
                    for (int k = 1; k < 6; k++) {
                        int px = i + k * dx;
                        int py = j + k * dy;
                        if (px >= 0 && px < size && py >= 0 && py < size && current[px][py] == player) {
                            count++;
                        } else {
                            break; // Romper si no hay más fichas consecutivas.
                        }
                    }
                    if (count == 6) {
                        return true; // Se encontró una línea de 6 fichas.
                    }
                }
            }
        }
    }
    return false; // No se encontró ninguna línea ganadora.
}

// Realiza un movimiento en el tablero.
bool State::makeMove(int x, int y, int player) {
    if (x >= 0 && x < size && y >= 0 && y < size && current[x][y] == 0) {
        current[x][y] = player; // Colocar la ficha del jugador en la posición (x, y).
        return true; // Movimiento válido.
    }
    return false; // Movimiento inválido.
}

// Obtiene una lista de movimientos disponibles en el tablero.
vector<pair<int, int>> State::getAvailableMoves() {
    std::vector<std::pair<int, int>> moves;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) {
                moves.push_back({i, j}); // Agregar posición vacía a la lista de movimientos.
            }
        }
    }
    return moves;
}

// Verifica si el juego ha terminado (victoria de un jugador o empate).
bool State::isTerminal() {
    return checkWin(1) || checkWin(2) || getAvailableMoves().empty();
}

// Verifica si un movimiento específico resulta en una victoria para un jugador.
bool State::checkWinAfterMove(int x, int y, int player) {
    State tempState = *this; // Copia temporal del estado actual.
    tempState.makeMove(x, y, player); // Aplicar el movimiento en la copia.
    return tempState.checkWin(player); // Verificar si el movimiento resulta en una victoria.
}

// Evalúa la puntuación de un movimiento en función de su impacto en el juego.
double State::evaluateMove(int x, int y) {
    double score = 0.0;

    // Direcciones posibles para evaluar patrones.
    vector<pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},          {0, 1},
        {1, -1},  {1, 0}, {1, 1}
    };

    //---- Caso 1: Piezas adyacentes del mismo tipo.
    int adjacentPieces = 0;
    for (const auto& [dx, dy] : directions) {
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < size && ny >= 0 && ny < size && current[nx][ny] == currentPlayer) {
            adjacentPieces++; // Contar fichas adyacentes del mismo jugador.
        }
    }
    score += adjacentPieces * 5; // Aumentar la puntuación por fichas adyacentes.

    //---- Caso 2: Bloqueo de líneas del oponente.
    int opponent = 3 - currentPlayer; // Identificar al oponente.
    int blockingScore = 0;

    for (const auto& [dx, dy] : directions) {
        int count = 0;
        int gaps = 0;

        // Verificar en la dirección (dx, dy).
        for (int k = 1; k < 6; k++) {
            int nx = x + k * dx;
            int ny = y + k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == opponent) {
                    count++; // Contar fichas del oponente.
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir un espacio vacío.
                } else {
                    break; // Romper si hay una ficha del jugador o demasiados espacios.
                }
            } else {
                break; // Romper si se sale del tablero.
            }
        }

        // Verificar en la dirección (-dx, -dy).
        for (int k = 1; k < 6; k++) {
            int nx = x - k * dx;
            int ny = y - k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == opponent) {
                    count++; // Contar fichas del oponente.
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir un espacio vacío.
                } else {
                    break; // Romper si hay una ficha del jugador o demasiados espacios.
                }
            } else {
                break; // Romper si se sale del tablero.
            }
        }

        if (count >= 4) { // Si el oponente tiene 4 o más fichas en línea.
            blockingScore += 1000; // Priorizar bloquear.
        } else if (count >= 3) { // Si el oponente tiene 3 fichas en línea.
            blockingScore += 50; // Aumentar puntuación por bloqueo.
        }
    }
    score += blockingScore; // Añadir puntuación de bloqueo al score total.

    //---- Caso 3: Extensión de líneas propias.
    int lineExtensionScore = 0;
    for (const auto& [dx, dy] : directions) {
        int lineLength = 1;
        int gaps = 0;

        // Verificar en la dirección (dx, dy).
        for (int k = 1; k < 6; k++) {
            int nx = x + k * dx;
            int ny = y + k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == currentPlayer) {
                    lineLength++; // Extender línea propia.
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir un espacio vacío.
                } else {
                    break; // Romper si hay una ficha del oponente o demasiados espacios.
                }
            } else {
                break; // Romper si se sale del tablero.
            }
        }

        // Verificar en la dirección (-dx, -dy).
        for (int k = 1; k < 6; k++) {
            int nx = x - k * dx;
            int ny = y - k * dy;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                if (current[nx][ny] == currentPlayer) {
                    lineLength++; // Extender línea propia.
                } else if (current[nx][ny] == 0 && gaps < 1) {
                    gaps++; // Permitir un espacio vacío.
                } else {
                    break; // Romper si hay una ficha del oponente o demasiados espacios.
                }
            } else {
                break; // Romper si se sale del tablero.
            }
        }

        if (lineLength >= 2) {
            lineExtensionScore += lineLength * 10; // Aumentar puntuación por extensión de línea.
        }
    }
    score += lineExtensionScore; // Añadir puntuación de extensión al score total.

    //---- Caso 4: Proximidad al centro del tablero.
    int center = size / 2;
    double distanceToCenter = sqrt((x - center) * (x - center) + (y - center) * (y - center));
    score += (size - distanceToCenter) * 2; // Aumentar puntuación por cercanía al centro.

    //---- Caso 5: Flexibilidad (movimientos que dejan más opciones abiertas).
    int flexibilityScore = 0;
    for (const auto& [dx, dy] : directions) {
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < size && ny >= 0 && ny < size && current[nx][ny] == 0) {
            flexibilityScore += 5; // Aumentar puntuación por movimientos que dejan opciones abiertas.
        }
    }
    score += flexibilityScore; // Añadir puntuación de flexibilidad al score total.

    return score; // Devolver la puntuación total del movimiento.
}

// Encuentra un movimiento que permita ganar o bloquear al oponente.
pair<int, int> State::findWinningOrBlockingMove(int player) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) { // Verificar solo posiciones vacías.
                for (const auto& [dx, dy] : directions) {
                    int count = 0;
                    int gaps = 0;

                    // Verificar en la dirección (dx, dy).
                    for (int k = 1; k < 6; k++) {
                        int px = i + k * dx;
                        int py = j + k * dy;
                        if (px >= 0 && px < size && py >= 0 && py < size) {
                            if (current[px][py] == player) {
                                count++; // Contar fichas del jugador.
                            } else if (current[px][py] == 0 && gaps < 1) {
                                gaps++; // Permitir un espacio vacío.
                            } else {
                                break; // Romper si hay una ficha del oponente o demasiados espacios.
                            }
                        } else {
                            break; // Romper si se sale del tablero.
                        }
                    }

                    // Verificar en la dirección (-dx, -dy).
                    for (int k = 1; k < 6; k++) {
                        int px = i - k * dx;
                        int py = j - k * dy;
                        if (px >= 0 && px < size && py >= 0 && py < size) {
                            if (current[px][py] == player) {
                                count++; // Contar fichas del jugador.
                            } else if (current[px][py] == 0 && gaps < 1) {
                                gaps++; // Permitir un espacio vacío.
                            } else {
                                break; // Romper si hay una ficha del oponente o demasiados espacios.
                            }
                        } else {
                            break; // Romper si se sale del tablero.
                        }
                    }

                    if (count >= 5) { // Si hay 5 fichas en línea, este movimiento es ganador o bloqueador.
                        return {i, j}; // Devolver la posición del movimiento.
                    }
                }
            }
        }
    }

    return {-1, -1}; // No se encontró ningún movimiento ganador o bloqueador.
}