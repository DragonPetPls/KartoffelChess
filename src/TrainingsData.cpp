//
// Created by fabian on 12/28/24.
//
#include "TrainingsData.h"

Game TrainingsData::getRandomPosition() {
    Engine e;
    Game g;
    g.loadStartingPosition();
    int counter = 0;
    while (g.getStatus() == ON_GOING) {
        counter++;
        Move move;
        int r = rand() % 100;
        if(r < RANDOM_MOVE_CHANCE_PERCENTAGE) {
            //Do random move
            auto next = g.getAllPseudoLegalMoves();
            while(true) {
                int random = rand() % next.moveCount;
                g.doMove(next.moves[random]);
                if(g.isPositionLegal()) {
                    break;
                }
                g.undoMove();
            }
        } else {
            //A decent move
            Move bestMove = e.getMove(g, 1);
            g.doMove(bestMove);
        }
    }

    int undoCount = rand() % counter;

    for(int i = 0; i < undoCount; i++) {
        g.undoMove();
    }

    return g;
}
