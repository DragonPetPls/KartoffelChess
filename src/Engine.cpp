//
// Created by fabian on 11/19/24.
//

#include "Engine.h"

Move Engine::getMove(Game g, int timeLeft, int tineIncrement, int timePerMove) {
    Move bestMove;

    Moves next = g.getAllPseudoLegalMoves();
    for (int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        if(!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        return next.moves[i];
    }

    return bestMove;
}

void Engine::stopSearch() {
    std::cout << "stopped" << std::endl;
}
