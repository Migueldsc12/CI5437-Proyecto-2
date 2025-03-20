#ifndef CONNECT6_H
#define CONNECT6_H

#include <vector>
#include <utility>

class State {
    public:
        int size;
        std::vector<std::vector<int>> current;
        int currentPlayer; // 1: Jugador humano, 2: Jugador agente
    
        State();
        bool checkWin(int player);
        bool makeMove(int x, int y, int player);
        std::vector<std::pair<int, int>> getAvailableMoves();
        bool isTerminal();
    };

#endif // STATE_H