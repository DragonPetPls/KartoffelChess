//
// Created by fabian on 11/21/24.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include "StaticEvaluation.h"
#include "../Game/Game.h"

constexpr int MAX_HISTORY = 75000;
constexpr int ENDGAME_MATERIAL = 4000;
constexpr int SAFETY_DIVISOR = 1500;
constexpr int MOBILITY = 1;

class Evaluation : public StaticEvaluation {
private:
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

    static int getMidgamePieceValue(bitboard hitmap, int index, piece p, color c);
    static int getEndgamePieceValue(bitboard hitmap, int index, piece p, color c);
    static int getKingSafety(bitboard hitmap, int index, piece p, color c, bitboard ownKingSpace, bitboard enemyKingSpace, const Game &g);
public:
    int evaluate(const Game &g) override;
};


#endif //EVALUATION_H
