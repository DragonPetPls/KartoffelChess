//
// Created by fabian on 11/19/24.
//

#ifndef ENGINE_H
#define ENGINE_H

#include "Game.h"
#include "Search.h"

class Engine {
private:
    Search search;
public:
    Move getMove(Game g, int timeLeft, int tineIncrement, int timePerMove);
    Move getMove(Game g, int depth);
    void stopSearch();
    void printPrincipalVariation(Game &g);
};



#endif //ENGINE_H
