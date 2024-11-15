//
// Created by fabian on 11/13/24.
//

#ifndef MAGICBITBOARDS_H
#define MAGICBITBOARDS_H

#include <vector>
#include "constants.h"

/*
 * This struct encodes everything necessary to represent a move
 */
struct Move{
    bitboard fromSquare;
    bitboard toSquare;
    piece startingPiece;
    piece endingPiece; //ending piece is the same as starting piece unless promotion
};

/*
 * This struct is a single entry in one of the lookup tables
 */
struct Entry {
    bitboard reachable = 0;
    std::vector<Move> moves;
    bool isSet = false;
};

/*
 * This struct stores all data for one piece type on one square
 */
struct MagicTableSquare {
    std::vector<Entry> entries;
    int indexShift;
    uint64_t magicNumber;
    bitboard blockerOverlay;
};

/*
 * Only used when generating magic bitboards
 */
struct calculationEntry {
    bitboard reachable = 0;
    bool isSet = false;
};

class MagicBitboards {
private:
    static MagicTableSquare bishopTable[64];

    static void initBishopTable();
    static bool isInit;

public:
    static std::vector<Move> getBishopMoves(bitboard hitmap, int index);
    static bitboard getBishopReachableSquares(bitboard hitmap, int index);
    static void init();


//Functions for generating magic numbers or initialisation
public:
    static void generateBishopMagicNumbers();
private:
    [[nodiscard]] static std::vector<Move> generateBishopMoves(bitboard square, int index, const bitboard& hitmap) ;
    static std::vector<bitboard> getAllBishopBlockerConfigurations(int index);
    static bitboard getBishopBlockerOverlay(int index);
};



#endif //MAGICBITBOARDS_H
