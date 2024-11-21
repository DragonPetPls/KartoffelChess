//
// Created by fabian on 11/19/24.
//

#include "Search.h"

/*
 * Performs a search until the stop variable is set to true
 */
void Search::search(Game &g, const std::atomic<bool> &stop) {

    int depth = 0;
    this->stop = &stop;

    //Iterative deepening, we perform 1000 cycles at max to avoid crashing the gui with consol outputs
    while (!stop && depth < 1000) {
        depth++;
        int eval = negamax(g, -INF, INF, depth);
        std::cout << depth << ": " << eval << std::endl;
    }
}

/*
 * Performs a recursive negamax search
 */
int Search::negamax(Game &g, int alpha, int beta, int depth) {
    if(*stop) {
        return 0;
    }

    //Checking if we entered a drawn position
    if(g.checkForRepetition()) {
        return 0;
    }

    if(depth <= 0) {
        return 0;
    }

    //actual search
    int bestScore = -INF;
    int bestIndex = 0;
    bool legalMoveExists = false;
    Moves next = g.getAllPseudoLegalMoves();
    for(int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        if(!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        legalMoveExists = true;

        int score = -negamax(g, alpha, beta, depth - 1);
        if(score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }

        g.undoMove();
    }

    //If we found no legal move we check if there might have been a checkmate
    if(legalMoveExists == false) {
        bool check = g.isKingInCheck(g.currentPlayer);
        bestScore = check ? -WIN : 0;
    }

    if(*stop == false) {
        if(transpositionTable.find(g.key()) != transpositionTable.end()) {
            transpositionTable[g.key()].bestMoveIndex = bestIndex;
        } else {
            Node n{bestScore, bestIndex};
            transpositionTable.insert(std::make_pair(g.key(), n));
        }

    }

    return bestScore;
}