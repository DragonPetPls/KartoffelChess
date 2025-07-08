//
// Created by fabian on 11/19/24.
//

#ifndef ENGINE_H
#define ENGINE_H

#include <thread>

#include "../Game/Game.h"
#include "Search.h"
#include "../Evaluation/StaticEvaluation.h"

class Engine {
private:
    StaticEvaluation *evaluator;
    std::atomic<bool> stop;
    std::unique_ptr<std::thread> thread;
    Search search;
public:
    Move getMove(Game g, int timeLeft, int tineIncrement, int timePerMove);
    Move getMove(Game g, int depth);
    void ponder(Game g);
    void stopSearch();
    void printPrincipalVariation(Game &g);
    void clearTranspositionTable() {
        search.clearTranspositionTable();
    }

    Engine(StaticEvaluation *evaluator) : search(*evaluator) {
        stop = false;
    }
};



#endif //ENGINE_H
