//
// Created by fabian on 11/5/24.
//

#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

#include <cstdint>

typedef uint64_t bitboard;
typedef uint8_t color;
typedef uint8_t piece;

enum PieceType{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Piece{
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,

    BLACK_PAWN = 8,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING,

    WHITE_PIECE = 0,
    BLACK_PIECE = 8,
    NO_PIECE = 14
};

enum Color{
    WHITE,
    BLACK
};

const uint8_t WHITE_SHORT_CASTLE_RIGHT = 1;
const uint8_t WHITE_LONG_CASTLE_RIGHT = 2;
const uint8_t BLACK_SHORT_CASTLE_RIGHT = 4;
const uint8_t BLACK_LONG_CASTLE_RIGHT = 8;

const bitboard WHITE_PAWN_STARTING_POSITION = 65280;
const bitboard WHITE_KNIGHT_STARTING_POSITION = 66;
const bitboard WHITE_BISHOP_STARTING_POSITION = 36;
const bitboard WHITE_ROOK_STARTING_POSITION = 129;
const bitboard WHITE_QUEEN_STARTING_POSITION = 8;
const bitboard WHITE_KING_STARTING_POSITION = 16;

const bitboard BLACK_PAWN_STARTING_POSITION = 71776119061217280;
const bitboard BLACK_KNIGHT_STARTING_POSITION = 4755801206503243776;
const bitboard BLACK_BISHOP_STARTING_POSITION = 2594073385365405696;
const bitboard BLACK_ROOK_STARTING_POSITION = 9295429630892703744;
const bitboard BLACK_QUEEN_STARTING_POSITION = 576460752303423488;
const bitboard BLACK_KING_STARTING_POSITION = 1152921504606846976;

const bitboard NO_EN_PASSANT = 0;

const piece COLOR_TO_PIECE[2] = {WHITE_PIECE, BLACK_PIECE};
const int NUMBER_TO_CHAR[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
const int INT_TO_CHAR[6] = {'p', 'n', 'b', 'r', 'q', 'k'};

const bitboard SHORT_CASTLE_ROOK[2] = {160, 11529215046068469760};
const bitboard LONG_CASTLE_ROOK[2] = {9, 648518346341351424};

const bitboard SHORT_CASTLE_KING = 5764607523034234960;
const bitboard LONG_CASTLE_KING = 1441151880758558740;

const bitboard WHITE_SHORT_CASTLE_RIGHTS_MASK = 144;
const bitboard WHITE_LONG_CASTLE_RIGHTS_MASK = 17;
const bitboard BLACK_SHORT_CASTLE_RIGHTS_MASK = 10376293541461622784;
const bitboard BLACK_LONG_CASTLE_RIGHTS_MASK = 1224979098644774912;

const bitboard BLACK_EN_PASSANT_ROWS = 71777214277877760;
const bitboard WHITE_EN_PASSANT_ROWS = 4278255360;

#endif //SRC_CONSTANTS_H
