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

enum Results {
    WHITE_WON,
    BLACK_WON,
    DRAW,
    ON_GOING,
    UNKNOWN
};

constexpr int INF = 1000000;

constexpr int NUMBER_OF_ZOBRIST_KEYS = 900;

constexpr int REPETITIONS_TILL_DRAW = 2;

constexpr uint8_t WHITE_SHORT_CASTLE_RIGHT = 1;
constexpr uint8_t WHITE_LONG_CASTLE_RIGHT = 2;
constexpr uint8_t BLACK_SHORT_CASTLE_RIGHT = 4;
constexpr uint8_t BLACK_LONG_CASTLE_RIGHT = 8;

constexpr bitboard WHITE_PAWN_STARTING_POSITION = 65280ULL;
constexpr bitboard WHITE_KNIGHT_STARTING_POSITION = 66ULL;
constexpr bitboard WHITE_BISHOP_STARTING_POSITION = 36ULL;
constexpr bitboard WHITE_ROOK_STARTING_POSITION = 129ULL;
constexpr bitboard WHITE_QUEEN_STARTING_POSITION = 8ULL;
constexpr bitboard WHITE_KING_STARTING_POSITION = 16ULL;

constexpr bitboard BLACK_PAWN_STARTING_POSITION = 71776119061217280ULL;
constexpr bitboard BLACK_KNIGHT_STARTING_POSITION = 4755801206503243776ULL;
constexpr bitboard BLACK_BISHOP_STARTING_POSITION = 2594073385365405696ULL;
constexpr bitboard BLACK_ROOK_STARTING_POSITION = 9295429630892703744ULL;
constexpr bitboard BLACK_QUEEN_STARTING_POSITION = 576460752303423488ULL;
constexpr bitboard BLACK_KING_STARTING_POSITION = 1152921504606846976ULL;

constexpr bitboard NO_EN_PASSANT = 0ULL;

constexpr piece COLOR_TO_PIECE[2] = {WHITE_PIECE, BLACK_PIECE};
constexpr char NUMBER_TO_CHAR[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
constexpr char INT_TO_CHAR[6] = {'p', 'n', 'b', 'r', 'q', 'k'};

constexpr bitboard SHORT_CASTLE_ROOK[2] = {160, 11529215046068469760ULL};
constexpr bitboard LONG_CASTLE_ROOK[2] = {9, 648518346341351424ULL};

constexpr int SHORT_CASTLE_ROOK_STARTING_INDEX[2] = {7, 63};
constexpr int SHORT_CASTLE_ROOK_ENDING_INDEX[2] = {5, 61};
constexpr int LONG_CASTLE_ROOK_STARTING_INDEX[2] = {0, 56};
constexpr int LONG_CASTLE_ROOK_ENDING_INDEX[2] = {3, 59};

constexpr bitboard SHORT_CASTLE_KING = 5764607523034234960ULL;
constexpr bitboard LONG_CASTLE_KING = 1441151880758558740ULL;

constexpr bitboard BOTTOM_ROW = 255ULL;
constexpr bitboard TOP_ROW = 18374686479671623680ULL;

constexpr bitboard WHITE_SHORT_CASTLE_RIGHTS_MASK = 144ULL;
constexpr bitboard WHITE_LONG_CASTLE_RIGHTS_MASK = 17ULL;
constexpr bitboard BLACK_SHORT_CASTLE_RIGHTS_MASK = 10376293541461622784ULL;
constexpr bitboard BLACK_LONG_CASTLE_RIGHTS_MASK = 1224979098644774912ULL;

constexpr bitboard BLACK_EN_PASSANT_ROWS = 71777214277877760ULL;
constexpr bitboard WHITE_EN_PASSANT_ROWS = 4278255360ULL;

constexpr int MAX_GAME_LENGTH = 1000;

constexpr bitboard BACK_ROWS = 18374686479671623935ULL;

//These boards are used by getIndex to get the index of a square on a bitboard
constexpr bitboard INDEX_BOARDS[] = {4294967295ULL, 281470681808895ULL, 71777214294589695ULL, 1085102592571150095ULL, 3689348814741910323ULL, 6148914691236517205ULL};

//These boards can be used to check if there is a bit within some of the next elements
constexpr bitboard NEXT_INDEX_BOARDS[] = {4294967295ULL, 65535ULL, 255ULL, 15ULL, 3ULL, 1ULL};

constexpr int ZOBRIST_COLOR_INDEX = 768;
constexpr int ZOBRIST_WHITE_SHORT_CASTLE_INDEX = 769;
constexpr int ZOBRIST_WHITE_LONG_CASTLE_INDEX = 770;
constexpr int ZOBRIST_BLACK_SHORT_CASTLE_INDEX = 771;
constexpr int ZOBRIST_BLACK_LONG_CASTLE_INDEX = 772;
constexpr int ZOBRIST_EN_PASSANT_INDEX = 772;

#endif //SRC_CONSTANTS_H
