//
// Created by fabian on 11/21/24.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include "../Game/Game.h"
#include "../Tuner/parameters.h"

constexpr int MAX_HISTORY = 75000;
constexpr int SAFETY_DELTA_MARGIN = 200;
constexpr int ENDGAME_MATERIAL = 6766; //6459 - 7044
constexpr int SAFETY_DIVISOR = 4574; //4430 - 4718
constexpr int MOBILITY = 117; //117 - 117
constexpr int SAFETY_EVAL_MARGIN = 200;

class Evaluation {
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

    static int getIndex(int i, color c);

public:
    static const int MIDGAME_PIECE_VALUES[6];
    static const int ENDGAME_PIECE_VALUES[6];
    static const int FAST_PIECE_VALUES[6];

public:
    static int evaluate(const Game& game, int delta);
    static void setupEvaluation(Game &g);
    static void updateEvaluation(Game &g, const LastMove& m);
    static uint64_t evaluationCount;
};


#endif //EVALUATION_H
