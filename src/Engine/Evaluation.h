//
// Created by fabian on 11/21/24.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include "../Game/Game.h"

constexpr int MAX_HISTORY = 75000;
constexpr int SAFETY_DELTA_MARGIN = 200;
constexpr int ENDGAME_MATERIAL = 7811;
constexpr int SAFETY_DIVISOR = 1500;
constexpr int ENDGAME_MOBILITY = 100;
constexpr int MIDGAME_MOBILITY = 100;
constexpr int SAFETY_EVAL_MARGIN = 200;

class Evaluation {
private:

    struct Order {
        int index;
        int value;
    };

    static const int MIDGAME_PAWN_TABLE[64];
    static const int ENDGAME_PAWN_TABLE[64];
    static const int MIDGAME_KNIGHT_TABLE[64];
    static const int ENDGAME_KNIGHT_TABLE[64];
    static const int MIDGAME_BISHOP_TABLE[64];
    static const int ENDGAME_BISHOP_TABLE[64];
    static const int MIDGAME_ROOK_TABLE[64];
    static const int ENDGAME_ROOK_TABLE[64];
    static const int MIDGAME_QUEEN_TABLE[64];
    static const int ENDGAME_QUEEN_TABLE[64];
    static const int MIDGAME_KING_TABLE[64];
    static const int ENDGAME_KING_TABLE[64];

    static const int MIDGAME_PIECE_VALUES[6];
    static const int ENDGAME_PIECE_VALUES[6];
    static const int FAST_PIECE_VALUES[6];

    static int getMidgamePieceValue(bitboard hitmap, int index, piece p, color c);
    static int getEndgamePieceValue(bitboard hitmap, int index, piece p, color c);
    static int getMoveValue(const Move &move, const Game &g, Moves &killerMoves, int historyTable[6][64]);
    static int getKingSafety(bitboard hitmap, int index, piece p, color c, bitboard ownKingSpace, bitboard enemyKingSpace, const Game &g);
    static int fastEvalation(const Game &g);
public:

    static int TEST_CONSTANT;

    static int evaluate(const Game &g, int alpha);
    static std::vector<int> rankMoves(const Game &g, const Moves &moves, int prevBestIndex, Moves &killerMoves, int historyTable[6][64]);
    static std::vector<int> rankCaptures(const Game &g, const Moves &moves, int delta);
    static uint64_t evaluationCount;
};


#endif //EVALUATION_H
