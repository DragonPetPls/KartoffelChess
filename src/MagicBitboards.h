//
// Created by fabian on 11/13/24.
//

#ifndef MAGICBITBOARDS_H
#define MAGICBITBOARDS_H

#include <vector>
#include "constants.h"
#include "Move.h"

/*
 * This struct is a single entry in one of the lookup tables
 */
struct Entry {
    bitboard reachable;
    Moves *moves;
    bool isSet;
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
    static MagicTableSquare rookTable[64];
    static Entry knightTable[64];
    static Entry kingTable[64];

    static void initBishopTable();
    static void initRookTable();
    static void initKnightTable();
    static void initKingTable();

    static bool isInit;

public:
    static void appendBishopMoves(bitboard hitmap, int index, Moves &moves, bitboard collisions);
    static bitboard getBishopReachableSquares(bitboard hitmap, int index);

    static void appendRookMoves(bitboard hitmap, int index, Moves &moves, bitboard collisions);
    static bitboard getRookReachableSquares(bitboard hitmap, int index);

    static void appendKnightMoves(int index, Moves &moves, bitboard collisions);
    static bitboard getKnightReachableSquares(int index);

    static void appendKingMoves(int index, Moves &moves, bitboard collisions);
    static bitboard getKingReachableSquares(int index);

    static void init();


//Functions for generating magic numbers or initialisation
public:
    static void generateBishopMagicNumbers();

    static void generateRookMagicNumbers();
private:
    [[nodiscard]] static Moves generateBishopMoves(bitboard square, int index, const bitboard& hitmap) ;
    static std::vector<bitboard> getAllBishopBlockerConfigurations(int index);
    static bitboard getBishopBlockerOverlay(int index);

    [[nodiscard]] static Moves generateRookMoves(bitboard square, int index, const bitboard& hitmap) ;
    static std::vector<bitboard> getAllRookBlockerConfigurations(int index);
    static bitboard getRookBlockerOverlay(int index);

    [[nodiscard]] static Moves generateKnightMoves(bitboard square, int index);

};



#endif //MAGICBITBOARDS_H
