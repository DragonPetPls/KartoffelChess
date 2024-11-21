//
// Created by fabian on 11/21/24.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include "Game.h"


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
    static int getMoveValue(const Move &move, const Game &g);
public:
    static int evaluate(const Game &g);
    static std::vector<int> rankMoves(const Game &g, const Moves &moves, int prevBestIndex);
    static uint64_t evaluationCount;
};


#endif //EVALUATION_H
