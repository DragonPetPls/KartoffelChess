//
// Created by fabian on 11/19/24.
//

#include "Search.h"

#include "Evaluation.h"

/*
 * Performs a search until the stop variable is set to true
 */
void Search::search(Game &g, const std::atomic<bool> &stop) {

    transpositionTable.clear();

    int depth = 0;
    this->stop = &stop;

    //Iterative deepening, we perform 1000 cycles at max to avoid crashing the gui with consol outputs
    while (!stop && depth < 1000) {
        depth++;
        int eval = negamax(g, -INF, INF, depth, depth);
        std::cout << depth << ": " << eval << std::endl;
    }
}

/*
 * Performs a search until the set depth is reached
 */
void Search::searchToDepth(Game &g, int toDepth) {
    std::atomic<bool> stop(false);
    this->stop = &stop;
    transpositionTable.clear();

    int depth = 0;

    //Iterative deepening, we perform 1000 cycles at max to avoid crashing the gui with consol outputs
    while (depth < toDepth) {
        depth++;
        int eval = negamax(g, -INF, INF, depth, depth);
        std::cout << depth << ": " << eval << std::endl;
    }
}

/*
 * Performs a recursive negamax search, depth is how much deeper should be searched, maxDepth is how far it should look at max
 */
int Search::negamax(Game &g, int alpha, int beta, int depth, int maxDepth) {
    if(*stop) {
        return 0;
    }

    //Checking if we entered a drawn position
    if(depth != maxDepth && g.checkForRepetition()) {
        return 0;
    }

    //Checking the position in the transposition table
    int originalAlpha = alpha;
    int prevBestIndex = INF;
    if(transpositionTable.find(g.key()) != transpositionTable.end()) {
        Node &n = transpositionTable[g.key()];
        if(n.depth >= depth) {
            if(n.flag == EXACT) {
                return n.value;
            }
            if(n.flag == LOWERBOUND) {
                alpha = std::max(alpha, n.value);
            } else if (n.flag == UPPERBOUND) {
                beta = std::min(beta, n.value);
            }
            if(alpha >= beta) return n.value;
        }
        prevBestIndex = transpositionTable[g.key()].bestMoveIndex;
    }

    if(depth <= 0) {
        int eval = Evaluation::evaluate(g);
        Node n{eval, INF, EXACT, 0};
        if(transpositionTable.find(g.key()) != transpositionTable.end()) {
            transpositionTable[g.key()] = n;
        } else {
            transpositionTable.insert(std::make_pair(g.key(), n));
        }
        return eval;
    }

    //actual search
    int bestScore = -INF;
    int bestIndex = 0;
    bool legalMoveExists = false;
    Moves next = g.getAllPseudoLegalMoves();
    auto order = Evaluation::rankMoves(g, next, prevBestIndex);

    for(int index = 0; index < next.moveCount; index++) {

        int &i = order[index];

        g.doMove(next.moves[i]);
        if(!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        legalMoveExists = true;

        int score = -negamax(g, -beta, -alpha, depth - 1, maxDepth);
        g.undoMove();

        if(score > bestScore) {
            bestIndex = i;
            bestScore = score;
            if(bestScore > alpha) {
                alpha = bestScore;
            }
        }
        if(alpha >= beta) {
            break;
        }
    }

    //If we found no legal move we check if there might have been a checkmate
    if(legalMoveExists == false) {
        bool check = g.isKingInCheck(g.currentPlayer);
        bestScore = check ? -WIN + g.getGameHistoryCounter(): 0; //We add the game history counter to favor quicker checkmates
    }

    //We store stuff in the lookup table only if the time is not up
    if(*stop == false) {
        char flag;
        if(bestScore <= originalAlpha) {
            flag = UPPERBOUND;
        } else if (bestScore > beta) {
            flag = LOWERBOUND;
        } else {
            flag = EXACT;
        }
        Node n{bestScore, bestIndex, flag, depth};

        if(transpositionTable.find(g.key()) != transpositionTable.end()) {
            transpositionTable[g.key()] = n;
        } else {
            transpositionTable.insert(std::make_pair(g.key(), n));
        }
    }

    return bestScore;
}