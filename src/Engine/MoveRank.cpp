//
// Created by fabian on 30.11.25.
//

#include "MoveRank.h"

#include <algorithm>

#include "Evaluation.h"

/*
 * Ranks captures (and promotions)
 * Excludes all move that dont have a chance to improve the position by delta
 */
std::vector<int> MoveRank::rankCaptures(const Game &g, const Moves &moves, int delta) {
    std::vector<Order> order;
    order.reserve(moves.moveCount);

    for (int i = 0; i < moves.moveCount; i++) {
        int value = 0;
        piece capturedPiece = g.getPiece(moves.moves[i].toSquare);
        capturedPiece &= ~BLACK_PIECE; //Removing color
        //Captures and promotions first
        if (capturedPiece < 6) {
            value += 100000 + 2 * Evaluation::MIDGAME_PIECE_VALUES[capturedPiece];
            value += -Evaluation::MIDGAME_PIECE_VALUES[moves[i].startingPiece] * g.isSquareUnderAttack(Game::getIndex(moves[i].toSquare), 1 - g.currentPlayer);
            if (delta > Evaluation::MIDGAME_PIECE_VALUES[capturedPiece] + SAFETY_DELTA_MARGIN) {
                continue;
            }
        }
        if (moves.moves[i].startingPiece != moves.moves[i].endingPiece) {
            value += 100000 + 2 * Evaluation::MIDGAME_PIECE_VALUES[moves.moves[i].endingPiece] - Evaluation::MIDGAME_PIECE_VALUES[moves[i].startingPiece];
            value += -Evaluation::MIDGAME_PIECE_VALUES[moves[i].endingPiece] * g.isSquareUnderAttack(Game::getIndex(moves[i].toSquare), 1 - g.currentPlayer);
            if (delta > Evaluation::MIDGAME_PIECE_VALUES[capturedPiece] + SAFETY_DELTA_MARGIN) {
                continue;
            }
        }
        order.push_back(Order{i, value});
    }

    std::sort(order.begin(), order.end(), [](const Order &a, const Order &b) {
        //This line is highlighted
        return a.value > b.value;
    });

    std::vector<int> result;
    result.resize(order.size());
    for (int i = 0; i < order.size(); i++) {
        result[i] = order[i].index;
    }
    return result;
}

/*
 * Returns a vector of ints in the order that moves should be searched further
 * int& killerMoveCutoff will we the index of the last move that was part of the killer or better
 */
std::vector<int> MoveRank::rankMoves(const Game &g, const Moves &moves, int prevBestIndex, Moves &killerMoves,
                                       double historyTable[6][64], int& killerMoveCutoff) {
    std::vector<Order> order;
    order.reserve(moves.moveCount);

    for (int i = 0; i < moves.moveCount; i++) {
        int value = 0;
        if (i == prevBestIndex) {
            value = 1000000;
        } else {
            value = getMoveValue(moves[i], g, killerMoves, historyTable);
        }
        order.push_back(Order{i, value});
    }

    std::sort(order.begin(), order.end(), [](const Order &a, const Order &b) {
        return a.value > b.value;
    });

    std::vector<int> result;
    result.resize(order.size());
    killerMoveCutoff = 0;
    for (int i = 0; i < order.size(); i++) {
        result[i] = order[i].index;
        if (order[i].value > 70000) {
            killerMoveCutoff = i;
        }
    }
    return result;
}

/*
 * returns a value that if represents how high on the priority list searching this move should be
 */
int MoveRank::getMoveValue(const Move &move, const Game &g, Moves &killerMoves, double historyTable[6][64]) {
    int value = 0;

    piece capturedPiece = g.getPiece(move.toSquare);
    capturedPiece &= ~BLACK_PIECE; //Removing color
    //Captures and promotions first
    bool captureOrPromotion = false;
    if (capturedPiece < 6) {
        value += 100000 + 5 * Evaluation::MIDGAME_PIECE_VALUES[capturedPiece];
        value += -Evaluation::MIDGAME_PIECE_VALUES[move.startingPiece] * g.isSquareUnderAttack(Game::getIndex(move.toSquare), 1 - g.currentPlayer);
        captureOrPromotion = true;
    }
    if (move.startingPiece != move.endingPiece) {
        value += 100000 + 5 * Evaluation::MIDGAME_PIECE_VALUES[move.endingPiece];
        value += -Evaluation::MIDGAME_PIECE_VALUES[move.endingPiece] * g.isSquareUnderAttack(Game::getIndex(move.toSquare), 1 - g.currentPlayer);
        captureOrPromotion = true;
    }

    //Now we check for killer moves
    bool killer = false;
    for (int i = 0; i < killerMoves.moveCount; i++) {
        if (killerMoves.moves[i] == move) {
            value += 1;
            killer = true;
        }
    }
    value += 80000 * killer * !captureOrPromotion;
    if (captureOrPromotion || killer) {
        return value;
    }
    int index = Game::getIndex(move.toSquare);
    value += historyTable[move.startingPiece][index];
    return value;
}