//
// Search Engine Core Implementation
//

#include "Search.h"
#include "../Communication/Writer.h"
#include "Evaluation.h"

Search::Search() {
    initLMR();
}

void Search::initLMR() {
    for (int depth = 1; depth < 64; depth++) {
        for (int moveIndex = 1; moveIndex < 64; moveIndex++) {
            LMRTable[depth][moveIndex] = static_cast<int>(0.5 + log(depth) * log(moveIndex) / 2.25);
        }
    }
}

/*
 * Ponders the given position
 */
void Search::ponder(Game g, const std::atomic<bool> &stop) {
    table.clear();
    this->stop = &stop;
    int depth = 0;
    lastCompletedBestMove = nullMove;
    bestRootMove = nullMove;

    std::fill(&killerMoves[0][0], &killerMoves[0][0] + sizeof(killerMoves) / sizeof(Move), nullMove);
    std::fill(&historyTable[0][0][0], &historyTable[0][0][0] + sizeof(historyTable) / sizeof(int), 0);

    Moves pseudoMoves = g.getAllPseudoLegalMoves();
    for (int i = 0; i < pseudoMoves.moveCount; i++) {
        g.doMove(pseudoMoves[i]);
        if (g.isPositionLegal()) {
            g.undoMove();
            lastCompletedBestMove = pseudoMoves[i];
            bestRootMove = pseudoMoves[i];
            break;
        }
        g.undoMove();
    }

    while ((this->stop == nullptr || !this->stop->load()) && depth < 100) {
        depth++;
        int eval = negamax(g, -INF, INF, depth, 0, true);
        if (this->stop == nullptr || !this->stop->load()) {
            if (bestRootMove.fromSquare != 0 || bestRootMove.toSquare != 0) {
                lastCompletedBestMove = bestRootMove;
            }
            Writer::print("info depth " + std::to_string(depth) + " score cp " + std::to_string(eval), "info");
        }
    }
}

/*
 * Performs a search until the stop variable is set to true
 */
void Search::search(Game &g, const std::atomic<bool> &stop) {
    this->stop = &stop;
    int depth = 0;
    int prevEval = 0;
    lastCompletedBestMove = nullMove;
    bestRootMove = nullMove;

    std::fill(&killerMoves[0][0], &killerMoves[0][0] + sizeof(killerMoves) / sizeof(Move), nullMove);
    std::fill(&historyTable[0][0][0], &historyTable[0][0][0] + sizeof(historyTable) / sizeof(int), 0);

    Moves pseudoMoves = g.getAllPseudoLegalMoves();
    for (int i = 0; i < pseudoMoves.moveCount; i++) {
        g.doMove(pseudoMoves[i]);
        if (g.isPositionLegal()) {
            g.undoMove();
            lastCompletedBestMove = pseudoMoves[i];
            bestRootMove = pseudoMoves[i];
            break;
        }
        g.undoMove();
    }

    while ((this->stop == nullptr || !this->stop->load()) && depth < 100) {
        depth++;
        int alpha = -INF;
        int beta = INF;
        int delta = 30;

        if (depth >= 4) {
            alpha = std::max(-INF, prevEval - delta);
            beta = std::min(INF, prevEval + delta);
        }

        while (this->stop == nullptr || !this->stop->load()) {
            int eval = negamax(g, alpha, beta, depth, 0, true);
            if (this->stop != nullptr && this->stop->load()) break;

            if (eval <= alpha) {
                alpha = std::max(-INF, alpha - delta);
                delta *= 2;
            } else if (eval >= beta) {
                beta = std::min(INF, beta + delta);
                delta *= 2;
            } else {
                prevEval = eval;
                if (bestRootMove.fromSquare != 0 || bestRootMove.toSquare != 0) {
                    lastCompletedBestMove = bestRootMove;
                }
                Writer::print("info depth " + std::to_string(depth) + " score cp " + std::to_string(eval), "info");
                break;
            }
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
    int prevEval = 0;
    lastCompletedBestMove = nullMove;
    bestRootMove = nullMove;

    std::fill(&killerMoves[0][0], &killerMoves[0][0] + sizeof(killerMoves) / sizeof(Move), nullMove);
    std::fill(&historyTable[0][0][0], &historyTable[0][0][0] + sizeof(historyTable) / sizeof(int), 0);

    Moves pseudoMoves = g.getAllPseudoLegalMoves();
    for (int i = 0; i < pseudoMoves.moveCount; i++) {
        g.doMove(pseudoMoves[i]);
        if (g.isPositionLegal()) {
            g.undoMove();
            lastCompletedBestMove = pseudoMoves[i];
            bestRootMove = pseudoMoves[i];
            break;
        }
        g.undoMove();
    }

    while (depth < toDepth) {
        depth++;
        int alpha = -INF;
        int beta = INF;
        int delta = 30;

        if (depth >= 4) {
            alpha = std::max(-INF, prevEval - delta);
            beta = std::min(INF, prevEval + delta);
        }

        while (true) {
            int eval = negamax(g, alpha, beta, depth, 0, true);
            if (eval <= alpha) {
                alpha = std::max(-INF, alpha - delta);
                delta *= 2;
            } else if (eval >= beta) {
                beta = std::min(INF, beta + delta);
                delta *= 2;
            } else {
                prevEval = eval;
                if (bestRootMove.fromSquare != 0 || bestRootMove.toSquare != 0) {
                    lastCompletedBestMove = bestRootMove;
                }
                Writer::print("info depth " + std::to_string(depth) + " score cp " + std::to_string(eval), "info");
                break;
            }
        }
    }
}

/*
 * Performs a recursive negamax search with PVS, LMR, NMP, and Transposition Table
 */
int Search::negamax(Game &g, int alpha, int beta, int depth, int ply, bool isPVNode) {
    if (stop != nullptr && stop->load()) {
        return 0;
    }

    if (ply > 0 && g.checkForRepetition()) {
        return 0;
    }

    if (ply >= MAX_PLY - 1) {
        return Evaluation::evaluate(g, alpha);
    }

    int originalAlpha = alpha;
    Move ttMove = nullMove;
    int ttScore = 0;

    if (table.probe(g.hashValue, ply, depth, alpha, beta, ttScore, ttMove)) {
        if (ply == 0 && (ttMove.fromSquare != 0 || ttMove.toSquare != 0)) {
            bestRootMove = ttMove;
        }
        return ttScore;
    }

    if (depth <= 0) {
        return quiescence(g, alpha, beta, depth, ply);
    }

    bool isCheck = g.isKingInCheck(g.currentPlayer);
    if (isCheck) depth++;

    // Null Move Pruning
    if (depth >= 3 && !isPVNode && !isCheck && (g.pieceBoards[COLOR_TO_PIECE[g.currentPlayer] | KNIGHT] |
        g.pieceBoards[COLOR_TO_PIECE[g.currentPlayer] | BISHOP] |
        g.pieceBoards[COLOR_TO_PIECE[g.currentPlayer] | ROOK] |
        g.pieceBoards[COLOR_TO_PIECE[g.currentPlayer] | QUEEN]) != 0) {

        g.doNullMove();
        int R = 2 + (depth > 6);
        int nmScore = -negamax(g, -beta, -beta + 1, depth - 1 - R, ply + 1, false);
        g.undoMove();

        if (stop != nullptr && stop->load()) return 0;

        if (nmScore >= beta) {
            return nmScore >= IS_MATE_SCORE ? beta : nmScore;
        }
    }

    Moves pseudoMoves = g.getAllPseudoLegalMoves();
    int scores[218];
    Evaluation::scoreMoves(g, pseudoMoves, ttMove, killerMoves[ply], historyTable, scores);

    int bestScore = -INF;
    Move bestMove = nullMove;
    int legalMoveCount = 0;

    for (int i = 0; i < pseudoMoves.moveCount; i++) {
        int bestIdx = i;
        for (int j = i + 1; j < pseudoMoves.moveCount; j++) {
            if (scores[j] > scores[bestIdx]) {
                bestIdx = j;
            }
        }
        std::swap(scores[i], scores[bestIdx]);
        std::swap(pseudoMoves[i], pseudoMoves[bestIdx]);

        const Move &move = pseudoMoves[i];
        g.doMove(move);

        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        legalMoveCount++;

        piece capturedPiece = g.getLastMove().capturedPiece;
        bool isQuiet = (capturedPiece == NO_PIECE) && (move.startingPiece == move.endingPiece);
        int score = 0;

        if (legalMoveCount == 1) {
            score = -negamax(g, -beta, -alpha, depth - 1, ply + 1, isPVNode);
        } else {
            int reduction = 0;
            if (depth >= 3 && legalMoveCount > 3 && isQuiet && !isCheck) {
                int moveIndexClamped = std::min(legalMoveCount, 63);
                int depthClamped = std::min(depth, 63);
                reduction = LMRTable[depthClamped][moveIndexClamped];
                if (isPVNode) reduction /= 2;
                reduction = std::min(depth - 2, reduction);
                reduction = std::max(0, reduction);
            }

            score = -negamax(g, -alpha - 1, -alpha, depth - 1 - reduction, ply + 1, false);

            if (score > alpha && (reduction > 0 || score < beta)) {
                score = -negamax(g, -beta, -alpha, depth - 1, ply + 1, isPVNode);
            }
        }

        g.undoMove();

        if (stop != nullptr && stop->load()) {
            return 0;
        }

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
            if (ply == 0) {
                bestRootMove = move;
            }
            if (score > alpha) {
                alpha = score;
            }
        }

        if (alpha >= beta) {
            if (isQuiet) {
                if (!(move == killerMoves[ply][0])) {
                    killerMoves[ply][1] = killerMoves[ply][0];
                    killerMoves[ply][0] = move;
                }
                int fromIdx = Game::getIndex(move.fromSquare);
                int toIdx = Game::getIndex(move.toSquare);
                historyTable[g.currentPlayer][fromIdx][toIdx] += depth * depth;
                if (historyTable[g.currentPlayer][fromIdx][toIdx] > 100000) {
                    historyTable[g.currentPlayer][fromIdx][toIdx] /= 2;
                }
            }
            break;
        }
    }

    if (legalMoveCount == 0) {
        if (isCheck) {
            bestScore = -WIN + ply;
        } else {
            bestScore = 0;
        }
    }

    if (stop == nullptr || !stop->load()) {
        uint8_t flag = (bestScore <= originalAlpha) ? UPPERBOUND : ((bestScore >= beta) ? LOWERBOUND : EXACT);
        table.store(g.hashValue, bestScore, bestMove, flag, depth, ply);
    }

    return bestScore;
}

/*
 * Performs a quiescence search to limit the horizon effect
 */
int Search::quiescence(Game &g, int alpha, int beta, int depth, int ply) {
    if (stop != nullptr && stop->load()) return 0;

    bool inCheck = g.isKingInCheck(g.currentPlayer);
    int standPat = -INF;

    if (!inCheck) {
        standPat = Evaluation::evaluate(g, alpha);
        if (standPat >= beta) return beta;
        if (alpha < standPat) alpha = standPat;
    }

    Moves pseudoMoves = inCheck ? g.getAllPseudoLegalMoves() : g.getAllPseudoLegalCaptures();
    int scores[218];
    if (inCheck) {
        Move noTT = nullMove;
        Move noKillers[2] = {nullMove, nullMove};
        Evaluation::scoreMoves(g, pseudoMoves, noTT, noKillers, historyTable, scores);
    } else {
        Evaluation::scoreCaptures(g, pseudoMoves, alpha - standPat, scores);
    }

    int legalMoveCount = 0;

    for (int i = 0; i < pseudoMoves.moveCount; i++) {
        int bestIdx = i;
        for (int j = i + 1; j < pseudoMoves.moveCount; j++) {
            if (scores[j] > scores[bestIdx]) bestIdx = j;
        }
        std::swap(scores[i], scores[bestIdx]);
        std::swap(pseudoMoves[i], pseudoMoves[bestIdx]);

        const Move &move = pseudoMoves[i];

        if (!inCheck && move.startingPiece == move.endingPiece && scores[i] < -500000) {
            continue;
        }

        g.doMove(move);

        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        legalMoveCount++;

        int score = -quiescence(g, -beta, -alpha, depth - 1, ply + 1);
        g.undoMove();

        if (stop != nullptr && stop->load()) return 0;

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    if (inCheck && legalMoveCount == 0) {
        return -WIN + ply;
    }

    return alpha;
}

/*
 * Print the continuation the engine currently thinks is best
 */
void Search::printPrincipleVariation(Game &g) {
    std::string out = "Principle Variation: ";
    Game copy = g;
    int depth = 0;

    while (depth < 20) {
        Move bestMove = nullMove;
        if (table.probeMove(copy.hashValue, bestMove) && (bestMove.fromSquare != 0 || bestMove.toSquare != 0)) {
            out += " " + std::to_string(Game::getIndex(bestMove.fromSquare)) + "->" + std::to_string(Game::getIndex(bestMove.toSquare));
            copy.doMove(bestMove);
            depth++;
        } else {
            break;
        }
    }
    Writer::print(out, "info");
}
