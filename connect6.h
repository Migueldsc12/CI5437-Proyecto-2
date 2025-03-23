#ifndef CONNECT6_H
#define CONNECT6_H

#include <vector>
#include <utility>

class State {
    public:
        int size;
        std::vector<std::vector<int>> current;
        int currentPlayer;
    
        State();
        bool checkWin(int player);
        bool makeMove(int x, int y, int player);
        std::vector<std::pair<int, int>> getAvailableMoves();
        bool isTerminal();
        std::pair<int, int> findWinningOrBlockingMove(int player);
        double evaluateMove(int x, int y);
        bool checkWinAfterMove(int x, int y, int player) ;
        int countInDirection(int x, int y, int dx, int dy, int player, int maxGaps);
        int countInBothDirections(int x, int y, int dx, int dy, int player, int maxGaps);
    };

#endif // STATE_H