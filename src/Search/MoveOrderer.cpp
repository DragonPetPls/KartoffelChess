//
// Created by fabia on 17.06.2025.
//

#include "MoveOrderer.h"

#include <algorithm>

const int MoveOrderer::MIDGAME_PIECE_VALUES[6] {
    82, 337, 365, 477, 1025,  0
};

/*
 * returns a value that if represents how high on the priority list searching this move should be
 */
int MoveOrderer::getMoveValue(const Move &move, const Game &g, Moves &killerMoves, int historyTable[6][64]) {
    int value = 0;

    piece capturedPiece = g.getPiece(move.toSquare);
    capturedPiece &= ~BLACK_PIECE; //Removing color
    //Captures and promotions first
    bool captureOrPromotion = false;
    if (capturedPiece < 6) {
        value += 100000 + MIDGAME_PIECE_VALUES[capturedPiece] - MIDGAME_PIECE_VALUES[move.startingPiece];
        captureOrPromotion = true;
    }
    if (move.startingPiece != move.endingPiece) {
        value += 100000 + MIDGAME_PIECE_VALUES[move.endingPiece] - MIDGAME_PIECE_VALUES[move.startingPiece];
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

    if (value != 0) {
        return value;
    }

    int index = Game::getIndex(move.toSquare);
    value += historyTable[move.startingPiece][index];
    return value;
}

/*
 * Returns a vector of ints in the order that moves should be searched further
 */
std::vector<int> MoveOrderer::rankMoves(const Game &g, const Moves &moves, int prevBestIndex, Moves &killerMoves,
                                       int historyTable[6][64]) {
    std::vector<Order> order;
    order.reserve(moves.moveCount);

    for (int i = 0; i < moves.moveCount; i++) {
        int value = 0;
        if (i == prevBestIndex) {
            value = 1000000;
        } else {
            value = getMoveValue(moves.moves[i], g, killerMoves, historyTable);
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
 * Ranks captures (and promotions)
 * Excludes all move that dont have a chance to improve the position by delta
 */
std::vector<int> MoveOrderer::rankCaptures(const Game &g, const Moves &moves, int delta) {
    std::vector<Order> order;
    order.reserve(moves.moveCount);

    for (int i = 0; i < moves.moveCount; i++) {
        int value = 0;
        piece capturedPiece = g.getPiece(moves.moves[i].toSquare);
        capturedPiece &= ~BLACK_PIECE; //Removing color
        //Captures and promotions first
        bool captureOrPromotion = false;
        if (capturedPiece < 6) {
            value += 100000 + MIDGAME_PIECE_VALUES[capturedPiece] - MIDGAME_PIECE_VALUES[moves.moves[i].startingPiece];
            captureOrPromotion = true;
            if (delta > MIDGAME_PIECE_VALUES[capturedPiece] + SAFETY_DELTA_MARGIN) {
                continue;
            }
        }
        if (moves.moves[i].startingPiece != moves.moves[i].endingPiece) {
            value += 100000 + MIDGAME_PIECE_VALUES[moves.moves[i].endingPiece] - MIDGAME_PIECE_VALUES[moves.moves[i].startingPiece];
            captureOrPromotion = true;
            if (delta > MIDGAME_PIECE_VALUES[capturedPiece] + SAFETY_DELTA_MARGIN) {
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