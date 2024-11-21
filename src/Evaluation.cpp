//
// Created by fabian on 11/21/24.
//

#include "Evaluation.h"

const int Evaluation::PAWN_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};

const int Evaluation::KNIGHT_TABLE[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

const int Evaluation::BISHOP_TABLE[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

const int Evaluation::ROOK_TABLE[64]= {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0
};

const int Evaluation::QUEEN_TABLE[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
};

const int Evaluation::KING_TABLE[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20
};

const int Evaluation::PIECE_VALUES[6] = {100, 320, 330, 500, 900, 0};




/*
 * Performs the static evaluation at the end of the search
 */
int Evaluation::evaluate(const Game &g) {
    int eval = 0;

    bitboard hitmap = 0;
    for(int i = 0; i < 6; i++) {
        hitmap |= g.pieceBoards[i | WHITE_PIECE];
        hitmap |= g.pieceBoards[i | BLACK_PIECE];
    }

    bitboard square = 1;
    for(int i = 0; i < 64; i++) {
        Game::fastForwardIndex(i, square, hitmap);
        if(i > 63) {
            break;
        }
        piece p = g.getPiece(square);
        color c = p & BLACK_PIECE ? BLACK : WHITE;
        p = p & ~BLACK_PIECE; //Removing the color of the piece
        int value = getPieceValue(i, p, c);
        eval += g.currentPlayer == c ? value : -value;
        square <<= 1;
    }

    return eval;
}

/*
 * Returns the value of a piece p of the color c on the index square, a high score is rewarded for a high value piece/square, regardless of color
 */
int Evaluation::getPieceValue(int index, piece p, color c) {
    int value = 0;
    value += PIECE_VALUES[p];

    int x = index % 8;
    int y = index / 8;

    index = c == WHITE ? x + 56 - 8 * y : x + 8 * y;

    switch (p) {
        case PAWN:
            value += PAWN_TABLE[index];
            break;
        case KNIGHT:
            value += KNIGHT_TABLE[index];
            break;
        case BISHOP:
            value += BISHOP_TABLE[index];
            break;
        case ROOK:
            value += ROOK_TABLE[index];
            break;
        case QUEEN:
            value += QUEEN_TABLE[index];
            break;
        case KING:
            value += KING_TABLE[index];
            break;
        default: break;
    }
    return value;
}
