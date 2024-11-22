//
// Created by fabian on 11/21/24.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include "Game.h"

constexpr int MAX_HISTORY = 75000;
constexpr int SAFETY_DELTA_MARGIN = 200;

class Evaluation {
private:

    struct Order {
        int index;
        int value;
    };

    static const int PAWN_TABLE[64];
    static const int KNIGHT_TABLE[64];
    static const int BISHOP_TABLE[64];
    static const int ROOK_TABLE[64];
    static const int QUEEN_TABLE[64];
    static const int KING_TABLE[64];

    static const int PIECE_VALUES[6];

    static int getPieceValue(int index, piece p, color c);
    static int getMoveValue(const Move &move, const Game &g, Moves &killerMoves, int historyTable[6][64]);
public:
    static int evaluate(const Game &g);
    static std::vector<int> rankMoves(const Game &g, const Moves &moves, int prevBestIndex, Moves &killerMoves, int historyTable[6][64]);
    static std::vector<int> rankCaptures(const Game &g, const Moves &moves, int delta);
    static uint64_t evaluationCount;
};


#endif //EVALUATION_H
