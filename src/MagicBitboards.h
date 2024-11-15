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
    static MagicTableSquare rookTable[64];
    static Entry knightTable[64];

    static void initBishopTable();
    static void initRookTable();
    static void initKnightTable();
    static bool isInit;

public:
    static std::vector<Move> getBishopMoves(bitboard hitmap, int index);
    static bitboard getBishopReachableSquares(bitboard hitmap, int index);

    static std::vector<Move> getRookMoves(bitboard hitmap, int index);
    static bitboard getRookReachableSquares(bitboard hitmap, int index);

    static std::vector<Move> getKnightMoves(int index);
    static bitboard getKnightReachableSquares(int index);

    static void init();


//Functions for generating magic numbers or initialisation
public:
    static void generateBishopMagicNumbers();

    static void generateRookMagicNumbers();
private:
    [[nodiscard]] static std::vector<Move> generateBishopMoves(bitboard square, int index, const bitboard& hitmap) ;
    static std::vector<bitboard> getAllBishopBlockerConfigurations(int index);
    static bitboard getBishopBlockerOverlay(int index);

    [[nodiscard]] static std::vector<Move> generateRookMoves(bitboard square, int index, const bitboard& hitmap) ;
    static std::vector<bitboard> getAllRookBlockerConfigurations(int index);
    static bitboard getRookBlockerOverlay(int index);

    [[nodiscard]] static std::vector<Move> generateKnightMoves(bitboard square, int index);

};



#endif //MAGICBITBOARDS_H
