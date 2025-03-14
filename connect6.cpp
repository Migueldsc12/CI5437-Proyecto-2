#include <iostream>
#include <vector>
#include "connect6.h"
using namespace std;

State::State() : size(19), current(vector<vector<int>>(size, vector<int>(size, 0))) {}

bool State::checkWin(int player){
    vector<pair<int,int>> directions= {{0,1}, {1,0}, {1,1},{1,-1}};
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++ ){
            if(current[i][j] == player){
                int count = 1;
                for(const auto&[dx,dy] : directions){
                    for(int k =1; k<6; k++){
                        int px = i + k * dx;
                        int py = j + k * dy;
                        if(px>= 0  && px < size && py < size && current[px][py]== player){
                            count++;
                        } else {
                            break;
                        }
                    }
                    if(count == 6){
                        return true;
                    }
                }
            }
        }
    }
    return false;
};

