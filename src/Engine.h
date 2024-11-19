//
// Created by fabian on 11/19/24.
//

#ifndef ENGINE_H
#define ENGINE_H

#include "Game.h"

class Engine {

public:
    Move getMove(Game g, int timeLeft, int tineIncrement, int timePerMove);
    void stopSearch();
};



#endif //ENGINE_H
