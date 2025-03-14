#ifndef CONNECT6_H
#define CONNECT6_H

#include <vector>
#include <utility>

class State {
public:
    int size;
    std::vector<std::vector<int>> current;

    State();

    bool checkWin(int player);
};

#endif // STATE_H
