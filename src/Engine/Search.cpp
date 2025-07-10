//
// Created by fabian on 11/19/24.
//

#include "Search.h"

#include "Evaluation.h"

/*
 * Ponders the given position
 */
void Search::ponder(Game g, const std::atomic<bool> &stop) {

    table.clear();

    int depth = 0;
    this->stop = &stop;

    //Iterative deepening, we perform 1000 cycles at max to avoid crashing the gui with consol outputs
    while (!stop && depth < 1000) {
        depth++;
        Moves killer;
        int eval = negamax(g, -INF, INF, depth, depth, killer, true);
        if(!stop) {
            std::cout << "info depth " << depth << " score cp " << eval << std::endl;
        }
    }

    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 64; j++) {
            historyTable[i][j] = 0;
        }
    }
}


/*
 * Performs a search until the stop variable is set to true
 */
void Search::search(Game &g, const std::atomic<bool> &stop) {

    int depth = 0;
    this->stop = &stop;

    //Iterative deepening, we perform 1000 cycles at max to avoid crashing the gui with consol outputs
    while (!stop && depth < 1000) {
        depth++;
        Moves killer;
        int eval = negamax(g, -INF, INF, depth, depth, killer, true);
        if(!stop) {
            std::cout << "info depth " << depth << " score cp " << eval << std::endl;
        }
    }
}

/*
 * Performs a search until the set depth is reached
 */
void Search::searchToDepth(Game &g, int toDepth) {
    std::atomic<bool> stop(false);
    this->stop = &stop;
    table.clear();

    int depth = 0;

    //Iterative deepening, we perform 1000 cycles at max to avoid crashing the gui with console outputs
    while (depth < toDepth) {
        depth++;
        Moves m;
        int eval = negamax(g, -INF, INF, depth, depth, m, true);
        std::cout << depth << ": " << eval << std::endl;
    }
}

/*
 * Performs a recursive negamax search, depth is how much deeper should be searched, maxDepth is how far it should look at max
 */
int Search::negamax(Game &g, int alpha, int beta, int depth, int maxDepth, Moves &killerMoves, bool isPVNode) {
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

    bool ttExists;
    Node ttNode = table.lookup(g, ttExists);

    if(ttExists) {
        if(ttNode.depth >= depth) {
            if(ttNode.flag == EXACT) {
               return ttNode.value;
            }
            if(ttNode.flag == LOWERBOUND) {
                alpha = std::max(alpha, ttNode.value);
            } else if (ttNode.flag == UPPERBOUND) {
                beta = std::min(beta, ttNode.value);
            }
            if(alpha >= beta) return ttNode.value;
        }
        prevBestIndex = ttNode.bestMoveIndex;
    }

    if(depth <= 0) {
        int eval = quiescence(g, alpha, beta, depth, maxDepth);
        char flag;
        if(eval <= originalAlpha) {
            flag = UPPERBOUND;
        } else if (eval >= beta) {
            flag = LOWERBOUND;
        } else {
            flag = EXACT;
        }
        table.write(g, eval, INF, flag, 0);

        return eval;
    }

    //Null move prunning
    if(depth < maxDepth * NULL_MOVE_FACTOR && !g.isKingInCheck(g.currentPlayer)) {
        g.doNullMove();
        Moves nmKillers;
        int nm = -negamax(g, -beta, -beta + 1, depth - NULL_MOVE_DEPTH_REDUCTION, maxDepth, nmKillers, false);
        g.undoMove();
        if(nm >= beta) return nm;
    }/* */

    //actual search
    int bestScore = -INF;
    int bestIndex = 0;
    bool legalMoveExists = false;
    Moves next = g.getAllPseudoLegalMoves();
    auto order = Evaluation::rankMoves(g, next, prevBestIndex, killerMoves, historyTable);
    Moves newKillerMoves;
    bool isCheck = g.isKingInCheck(g.currentPlayer);

    for(int index = 0; index < next.moveCount; index++) {
        int &i = order[index];

        g.doMove(next.moves[i]);
        if(!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        legalMoveExists = true;

        //principle variation search
        int score;
        if(index != 0){
            //TODO Add multithreading here
            int reduction = 1;
            reduction += 1 * (index >= (1 + 2 * isPVNode) && depth >= 3); //Late moves
            reduction -= 1 * isCheck; //Checks may be risky to reduce
            reduction = std::max(reduction, 1);

            score = -negamax(g, -alpha - 1, -alpha, depth - reduction, maxDepth, newKillerMoves, false);
            if(score > alpha) {
                score = -negamax(g, -beta, -alpha, depth - 1, maxDepth, newKillerMoves, isPVNode);
            }
        } else {
            score = -negamax(g, -beta, -alpha, depth - 1, maxDepth, newKillerMoves, isPVNode);
        }

        if(score > bestScore) {
            bestIndex = i;
            bestScore = score;
            if(bestScore > alpha) {
                alpha = bestScore;
            }
        }
        if(alpha >= beta) {
            killerMoves.moves[killerMoves.moveCount++] = next.moves[i];
            if(g.getLastMove().capturedPiece != NO_PIECE) {
                int clampedBonus = std::min(depth, MAX_HISTORY);
                int to = Game::getIndex(next.moves[i].toSquare);
                historyTable[next.moves[i].startingPiece][to] += clampedBonus - historyTable[next.moves[i].startingPiece][to] * abs(clampedBonus) / MAX_HISTORY;
            }
            g.undoMove();
            break;
        }
        g.undoMove();
    }

    //If we found no legal move we check if there might have been a checkmate
    if(legalMoveExists == false) {
        bool check = g.isKingInCheck(g.currentPlayer);
        bestScore = check ? -WIN + g.getGameHistoryCounter(): 0; //We add the game history counter to favor quicker checkmates
    }

    //We store stuff in the lookup table only if the time is not up
    if(!(*stop)) {
        char flag;
        if(bestScore <= originalAlpha) {
            flag = UPPERBOUND;
        } else if (bestScore >= beta) {
            flag = LOWERBOUND;
        } else {
            flag = EXACT;
        }

        table.write(g, bestScore, bestIndex, flag, depth);
    }

    return bestScore;
}

/*
 * Performs a quiescence search to limit the horizon effect
 */
int Search::quiescence(Game &g, int alpha, int beta, int depth, int maxDepth) {
    int standPat = Evaluation::evaluate(g, alpha);
    if(standPat >= beta) {
        return beta;
    }
    if(alpha < standPat) {
        alpha = standPat;
    }

    auto next = g.getAllPseudoLegalCaptures();
    auto order = Evaluation::rankCaptures(g, next, alpha - standPat);
    for(int index = 0; index < order.size(); index++) {
        int i = order[index];
        g.doMove(next.moves[i]);
        if(!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        int score = -quiescence(g, -beta, -alpha, depth - 1, maxDepth);
        g.undoMove();
        if(score >= beta) return beta;
        alpha = std::max(alpha, score);
    }
    return alpha;
}

/*
 * Print the continuation the engine currently thinks is best
 */
void Search::printPrincipleVariation(Game &g) {
    std::cout << "Principle Variation: ";
    int counter = 0;
    while(true) {
        if(g.getStatus() != ON_GOING) {
            break;
        }
        bool exists;
        Node n = table.lookup(g, exists);
        if(!exists) {
            break;
        }

        counter++;
        auto next = g.getAllPseudoLegalMoves();
        std::cout << Game::moveToString(next.moves[n.bestMoveIndex]) << " ";
        g.doMove(next.moves[n.bestMoveIndex]);
    }
    std::cout << std::endl;
    g.printGame();
    for(int i = 0; i < counter; i++) {
        g.undoMove();
    }

}
