//
// Created by fabian on 11/13/24.
//

#include "MagicBitboards.h"

#include <iostream>
#include <cmath>
#include <random>
#include <strings.h>

#include "magicConstants.h"

MagicTableSquare MagicBitboards::bishopTable[64];
MagicTableSquare MagicBitboards::rookTable[64];
Entry MagicBitboards::kingTable[64];
Entry MagicBitboards::knightTable[64];
bool MagicBitboards::isInit = false;

/*
 * Used to generate magic numbers for bishop, not neccesary after numbers ahave been calculated
 */
void MagicBitboards::generateBishopMagicNumbers() {
    const int STARTING_INDEX_SHIFT = 45;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

    //Preparing blockers and index
    std::vector<bitboard> blockers[64];
    uint64_t magicNumbers[64];
    int bestIndexShiftFound[64];
    for (int i = 0; i < 64; i++) {
        blockers[i] = getAllBishopBlockerConfigurations(i);
        bestIndexShiftFound[i] = STARTING_INDEX_SHIFT;
        magicNumbers[i] = 0;
    }

    //Searching for numbers
    while(true) {
        int index = rand()%64;
        int indexShift = bestIndexShiftFound[index] + 1;
        uint64_t magicNumber = dis(gen);
        bitboard square = ((bitboard) 1) << index;

        bool foundMagicNumber = true;
        uint64_t tableSize = ((uint64_t) 1) << (64 - indexShift);
        auto *table = new calculationEntry[tableSize];

        //now we just test all blockers
        for(bitboard b : blockers[index]) {
            uint64_t key = (b * magicNumber) >> indexShift;
            bitboard reachable = 0;
            auto moves = generateBishopMoves(square, index, b);
            for(int i = 0; i < moves.moveCount; i++) {
                reachable |= moves.moves[i].toSquare;
            }

            if(table[key].isSet == false) {
                table[key].isSet = true;
                table[key].reachable = reachable;
            } else if (table[key].reachable != reachable) {
                foundMagicNumber = false;
                break;
            }
        }

        if(foundMagicNumber) {
            magicNumbers[index] = magicNumber;
            bestIndexShiftFound[index] = indexShift;
            std::cout << "Found new number" << std::endl;
            for(int i = 0; i < 64; i++) {
                if(bestIndexShiftFound[i] != STARTING_INDEX_SHIFT) {
                    std::cout << "Index: " << i <<  " Index Shift: " << bestIndexShiftFound[i] << " Magic number: " << magicNumbers[i] << std::endl;
                }
            }
        }

        delete[] table;
    }
}

/*
 * Returns all pseudolegal bishop moves from the given square, used to initialise the magic bitboard table function
 */
Moves MagicBitboards::generateBishopMoves(bitboard square, int index, const bitboard &hitmap) {
    std::vector<Move> moves;
    std::vector<Move> furthestMoves;
    furthestMoves.reserve(4);
    moves.reserve(16);

    int x = index % 8;
    int y = index / 8;
    //Going over all 4 directions
    for(int i = 0; i < 4; i++){
        int vx = 1 * (i < 2) - 1 * (i >= 2);
        int vy = 1 * (i == 0 || i == 2) - 1 * (i == 1 || i == 3);

        //Going over distances
        for (int distance = 1; distance < 8; distance++){
            int tx = x + vx * distance;
            int ty = y + vy * distance;

            bitboard targetSquare = 1;
            targetSquare = targetSquare << (tx + 8 * ty);

            //Collisions and out of bounds
            if(tx < 0 || tx > 7 || ty < 0 || ty > 7){
                break;
            }

            //Adding the move
            Move m;
            m.toSquare = targetSquare;
            m.fromSquare = square;
            m.startingPiece = BISHOP;
            m.endingPiece = BISHOP;
            moves.push_back(m);

            //Checking for blockage
            if(targetSquare & hitmap){
                break;
            }
        }

        if (!moves.empty()) {
            furthestMoves.push_back(moves.back());
            moves.pop_back();
        }
    }

    moves.insert(moves.end(), furthestMoves.begin(), furthestMoves.end());
    Moves m;
    for(int i = 0; i < moves.size(); i++) {
        m.moves[m.moveCount++] = moves[i];
    }
    return m;
}

/*
 * Returns a vector containing all possible blocker configurations for a bishop on the index square
 */
std::vector<bitboard> MagicBitboards::getAllBishopBlockerConfigurations(int index) {
    int x = index % 8;
    int y = index / 8;
    std::vector<bitboard> blockers;
    bitboard pieceSquare = 1;
    pieceSquare = pieceSquare << index;

    //How many squares do we actually still place blockers in each direction
    int squaresInTopLeftDirection = std::min(x, 7 - y) - 1;
    int squaresInTopRightDirection = std::min(7 - x, 7 - y) - 1;
    int squaresInBottomLeftDirection = std::min(x, y) - 1;
    int squaresInBottomRightDirection = std::min(7 - x, y) - 1;

    //Going through those square combinations
    for(int topLeft = 0; topLeft < pow(2, squaresInTopLeftDirection); topLeft++) {
        for(int topRight = 0; topRight < pow(2, squaresInTopRightDirection); topRight++) {
            for(int bottomLeft = 0; bottomLeft < pow(2, squaresInBottomLeftDirection); bottomLeft++) {
                for(int bottomRight = 0; bottomRight < pow(2, squaresInBottomRightDirection); bottomRight++) {

                    //Actually placing those square down
                    bitboard b = 0;
                    for(int i = 0; i < squaresInTopLeftDirection; i++) {
                        b |= (pieceSquare << (8 * i - i + 7)) * ((topLeft >> i) & 1);
                    }
                    for(int i = 0; i < squaresInBottomLeftDirection; i++) {
                        b |= (pieceSquare >> (8 * i + i + 9)) * ((bottomLeft >> i) & 1);
                    }
                    for(int i = 0; i < squaresInTopRightDirection; i++) {
                        b |= (pieceSquare << (8 * i + i + 9)) * ((topRight >> i) & 1);
                    }
                    for(int i = 0; i < squaresInBottomRightDirection; i++) {
                        b |= (pieceSquare >> (8 * i - i + 7)) * ((bottomRight >> i) & 1);
                    }
                    blockers.push_back(b);
                }
            }
        }
    }

    return blockers;
}

/*
 * Initialising the table for bishops
 */
void MagicBitboards::initBishopTable() {

    for(int i = 0; i < 64; i++) {
        bishopTable[i].magicNumber = BISHOP_MAGIC_NUMBERS[i];
        bishopTable[i].indexShift = BISHOP_INDEX_SHIFTS[i];
        uint64_t tableSize = ((uint64_t) 1) << (64 - bishopTable[i].indexShift);
        bishopTable[i].entries.resize(tableSize);
        bishopTable[i].blockerOverlay = getBishopBlockerOverlay(i);
    }

    //Actually setting all moves up
    for(int i = 0; i < 64; i++) {
        bitboard square = ((bitboard) 1) << i;
        auto blockers = getAllBishopBlockerConfigurations(i);

        for(bitboard b : blockers) {
            uint64_t key = (b * bishopTable[i].magicNumber) >> bishopTable[i].indexShift;
            bitboard reachable = 0;
            auto moves = generateBishopMoves(square, i, b);
            for(int j = 0; j < moves.moveCount; j++) {
                reachable |= moves.moves[j].toSquare;
            }

            if(bishopTable[i].entries[key].isSet == false) {
                bishopTable[i].entries[key].isSet = true;
                bishopTable[i].entries[key].reachable = reachable;
                bishopTable[i].entries[key].moves = std::make_unique<Moves>();
                bishopTable[i].entries[key].moves->copy(moves);
            } else if (bishopTable[i].entries[key].reachable != reachable) {
                std::cout << "Error in MagicBitboards::initBishopTable()" << std::endl;
            }
        }
    }
}

/*
 * Returns all bishop moves from the lookup table, the last 4 moves should be checked in case the might attempt to capture an own piece
 */
void MagicBitboards::appendBishopMoves(bitboard hitmap, int index, Moves &moves, bitboard collisions) {

    bitboard blockers = hitmap & bishopTable[index].blockerOverlay;
    uint64_t key = (blockers * bishopTable[index].magicNumber) >> bishopTable[index].indexShift;
    moves.appendMovesWithoutCollision(*bishopTable[index].entries[key].moves, collisions, 4);
}

/*
 * Returns all squares a bishop could reach from the given position
 */
bitboard MagicBitboards::getBishopReachableSquares(bitboard hitmap, int index) {
    bitboard blockers = hitmap & bishopTable[index].blockerOverlay;
    uint64_t key = (blockers * bishopTable[index].magicNumber) >> bishopTable[index].indexShift;
    return bishopTable[index].entries[key].reachable;
}


/*
 * Returns a mask of all relevant square for blockers for a bishop
 */
bitboard MagicBitboards::getBishopBlockerOverlay(int index) {

    bitboard pieceSquare = 1;
    pieceSquare = pieceSquare << index;
    int x = index % 8;
    int y = index / 8;
    //Actually placing those square down
    int squaresInTopLeftDirection = std::min(x, 7 - y) - 1;
    int squaresInTopRightDirection = std::min(7 - x, 7 - y) - 1;
    int squaresInBottomLeftDirection = std::min(x, y) - 1;
    int squaresInBottomRightDirection = std::min(7 - x, y) - 1;

    bitboard b = 0;
    for(int i = 0; i < squaresInTopLeftDirection; i++) {
        b |= pieceSquare << (8 * i - i + 7);
    }
    for(int i = 0; i < squaresInBottomLeftDirection; i++) {
        b |= pieceSquare >> (8 * i + i + 9);
    }
    for(int i = 0; i < squaresInTopRightDirection; i++) {
        b |= pieceSquare << (8 * i + i + 9);
    }
    for(int i = 0; i < squaresInBottomRightDirection; i++) {
        b |= pieceSquare >> (8 * i - i + 7);
    }
    return b;
}

void MagicBitboards::init() {
    if(!isInit) {
        initBishopTable();
        initRookTable();
        initKnightTable();
        initKingTable();
        isInit = true;
    }
}

void MagicBitboards::generateRookMagicNumbers() {
    const int STARTING_INDEX_SHIFT = 45;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

    //Preparing blockers and index
    std::vector<bitboard> blockers[64];
    uint64_t magicNumbers[64];
    int bestIndexShiftFound[64];
    for (int i = 0; i < 64; i++) {
        blockers[i] = getAllRookBlockerConfigurations(i);
        bestIndexShiftFound[i] = STARTING_INDEX_SHIFT;
        magicNumbers[i] = 0;
    }

    //Searching for numbers
    while(true) {
        int index = rand()%64;
        int indexShift = bestIndexShiftFound[index] + 1;
        uint64_t magicNumber = dis(gen);
        bitboard square = ((bitboard) 1) << index;

        bool foundMagicNumber = true;
        uint64_t tableSize = ((uint64_t) 1) << (64 - indexShift);
        auto *table = new calculationEntry[tableSize];

        //now we just test all blockers
        for(bitboard b : blockers[index]) {
            uint64_t key = (b * magicNumber) >> indexShift;
            bitboard reachable = 0;
            auto moves = generateRookMoves(square, index, b);
            for(int i = 0; i < moves.moveCount; i++) {
                reachable |= moves.moves[i].toSquare;
            }

            if(table[key].isSet == false) {
                table[key].isSet = true;
                table[key].reachable = reachable;
            } else if (table[key].reachable != reachable) {
                foundMagicNumber = false;
                break;
            }
        }

        if(foundMagicNumber) {
            magicNumbers[index] = magicNumber;
            bestIndexShiftFound[index] = indexShift;
            std::cout << "Found new number" << std::endl;
            for(int i = 0; i < 64; i++) {
                if(bestIndexShiftFound[i] != STARTING_INDEX_SHIFT) {
                    std::cout << "Index: " << i <<  " Index Shift: " << bestIndexShiftFound[i] << " Magic number: " << magicNumbers[i] << std::endl;
                }
            }
        }

        delete[] table;
    }
}

/*
 * Returns all pseudolegal rook moves from the given square, used to init the rook table
 */
Moves MagicBitboards::generateRookMoves(bitboard square, int index, const bitboard &hitmap) {
    std::vector<Move> moves;
    std::vector<Move> furthestMoves;
    moves.reserve(16);

    int x = index % 8;
    int y = index / 8;
    //Going over all 4 directions
    for(int i = 0; i < 4; i++){
        int vx = 1 * (i == 0) - 1 * (i == 2);
        int vy = 1 * (i == 1) - 1 * (i == 3);

        //Going over distances
        for (int distance = 1; distance < 8; distance++){
            int tx = x + vx * distance;
            int ty = y + vy * distance;

            //Collisions and out of bounds
            if(tx < 0 || tx > 7 || ty < 0 || ty > 7){
                break;
            }

            bitboard targetSquare = 1;
            targetSquare = targetSquare << (tx + 8 * ty);

            //Adding the move
            Move m;
            m.toSquare = targetSquare;
            m.fromSquare = square;
            m.startingPiece = ROOK;
            m.endingPiece = ROOK;
            moves.push_back(m);

            //Checking for capture
            if(targetSquare & hitmap){
                break;
            }
        }

        if (!moves.empty()) {
            furthestMoves.push_back(moves.back());
            moves.pop_back();
        }
    }
    moves.insert(moves.end(), furthestMoves.begin(), furthestMoves.end());
    Moves m;
    for(int i = 0; i < moves.size(); i++) {
        m.moves[m.moveCount++] = moves[i];
    }
    return m;
}

/*
 * Returns a vector containing all possible blocker configurations for a rook on the index square
 */
std::vector<bitboard> MagicBitboards::getAllRookBlockerConfigurations(int index) {
    int x = index % 8;
    int y = index / 8;
    std::vector<bitboard> blockers;
    bitboard pieceSquare = 1;
    pieceSquare = pieceSquare << index;

    //How many squares do we actually still place blockers in each direction
    int squaresInTopDirection = 7 - y;
    int squaresInBottomDirection = y;
    int squaresInLeftDirection = x;
    int squaresInRightDirection = 7 - x;

    //Going through those square combinations
    for(int top = 0; top < pow(2, squaresInTopDirection); top++) {
        for(int left = 0; left < pow(2, squaresInLeftDirection); left++) {
            for(int right = 0; right < pow(2, squaresInRightDirection); right++) {
                for(int bottom = 0; bottom < pow(2, squaresInBottomDirection); bottom++) {

                    //Actually placing those square down
                    bitboard b = 0;
                    for(int i = 0; i < squaresInTopDirection; i++) {
                        b |= (pieceSquare << (8 * i + 8)) * ((top >> i) & 1);
                    }
                    for(int i = 0; i < squaresInLeftDirection; i++) {
                        b |= (pieceSquare >> (i+ 1)) * ((left >> i) & 1);
                    }
                    for(int i = 0; i < squaresInRightDirection; i++) {
                        b |= (pieceSquare << (i + 1)) * ((right >> i) & 1);
                    }
                    for(int i = 0; i < squaresInBottomDirection; i++) {
                        b |= (pieceSquare >> (8 * i + 8)) * ((bottom >> i) & 1);
                    }
                    blockers.push_back(b);
                }
            }
        }
    }

    return blockers;
}


/*
 * Returns a mask of all relevant square for blockers for a bishop
 */
bitboard MagicBitboards::getRookBlockerOverlay(int index) {

    bitboard pieceSquare = 1;
    pieceSquare = pieceSquare << index;
    int x = index % 8;
    int y = index / 8;
    //Actually placing those square down
    int squaresInTopDirection = 7 - y;
    int squaresInBottomDirection = y;
    int squaresInLeftDirection = x;
    int squaresInRightDirection = 7 - x;

    bitboard b = 0;
    for(int i = 0; i < squaresInTopDirection; i++) {
        b |= (pieceSquare << (8 * i + 8));
    }
    for(int i = 0; i < squaresInLeftDirection; i++) {
        b |= (pieceSquare >> (i+ 1));
    }
    for(int i = 0; i < squaresInRightDirection; i++) {
        b |= (pieceSquare << (i + 1));
    }
    for(int i = 0; i < squaresInBottomDirection; i++) {
        b |= (pieceSquare >> (8 * i + 8));
    }
    return b;
}

/*
 * Initialising the table for rook
 */
void MagicBitboards::initRookTable() {

    for(int i = 0; i < 64; i++) {
        rookTable[i].magicNumber = ROOK_MAGIC_NUMBERS[i];
        rookTable[i].indexShift = ROOK_INDEX_SHIFTS[i];
        uint64_t tableSize = ((uint64_t) 1) << (64 - rookTable[i].indexShift);
        rookTable[i].entries.resize(tableSize);
        rookTable[i].blockerOverlay = getRookBlockerOverlay(i);
    }

    //Actually setting all moves up
    for(int i = 0; i < 64; i++) {
        bitboard square = ((bitboard) 1) << i;
        auto blockers = getAllRookBlockerConfigurations(i);

        for(bitboard b : blockers) {
            uint64_t key = (b * rookTable[i].magicNumber) >> rookTable[i].indexShift;
            bitboard reachable = 0;
            auto moves = generateRookMoves(square, i, b);
            for(int i = 0; i < moves.moveCount; i++) {
                reachable |= moves.moves[i].toSquare;
            }

            if(rookTable[i].entries[key].isSet == false) {
                rookTable[i].entries[key].isSet = true;
                rookTable[i].entries[key].reachable = reachable;
                rookTable[i].entries[key].moves = std::make_unique<Moves>();
                rookTable[i].entries[key].moves->copy(moves);
            } else if (rookTable[i].entries[key].reachable != reachable) {
                std::cout << "Error in MagicBitboards::initRookTable() at " << i << std::endl;
            }
        }
    }
}

/*
 * Returns all rook moves from the lookup table, the last 4 moves should be checked in case the might attempt to capture an own piece
 */
void MagicBitboards::appendRookMoves(bitboard hitmap, int index, Moves &moves, bitboard collisions) {

    bitboard blockers = hitmap & rookTable[index].blockerOverlay;
    uint64_t key = (blockers * rookTable[index].magicNumber) >> rookTable[index].indexShift;
    moves.appendMovesWithoutCollision(*rookTable[index].entries[key].moves, collisions, 4);
}

/*
 * Returns all squares a rook could reach from the given position
 */
bitboard MagicBitboards::getRookReachableSquares(bitboard hitmap, int index) {
    bitboard blockers = hitmap & rookTable[index].blockerOverlay;
    uint64_t key = (blockers * rookTable[index].magicNumber) >> rookTable[index].indexShift;
    return rookTable[index].entries[key].reachable;
}

/*
 * Returns all pseudolegal knight moves from the given square, used to init the knight lookup tables
 */
Moves MagicBitboards::generateKnightMoves(bitboard square, int index) {
    std::vector<Move> moves;
    moves.reserve(8);

    int x = index % 8;
    int y = index / 8;

    for(int i = 0; i < 8; i++){
        //Going over directions
        int vx = 2 * (i == 0 || i == 1) + 1 * (i == 2 || i == 3) - 1 * (i == 4 || i == 5) - 2 * (i == 6 || i == 7);
        int vy = 2 * (i == 2 || i == 4) + 1 * (i == 0 || i == 6) - 1 * (i == 1 || i == 7) - 2 * (i == 3 || i == 5);

        //Generating target square
        int tx = x + vx;
        int ty = y + vy;
        if(tx < 0 || tx > 7 || ty < 0 || ty > 7){
            continue;
        }

        bitboard targetSquare = 1;
        targetSquare = targetSquare << (tx + 8 * ty);

        Move m;
        m.toSquare = targetSquare;
        m.fromSquare = square;
        m.startingPiece = KNIGHT;
        m.endingPiece = KNIGHT;
        moves.push_back(m);

    }

    Moves m;
    for(int i = 0; i < moves.size(); i++) {
        m.moves[m.moveCount++] = moves[i];
    }
    return m;
}

/*
 * Initialises the knight table
 */
void MagicBitboards::initKnightTable() {
    for(int i = 0; i < 64; i++) {
        bitboard square = ((bitboard) 1) << i;
        auto moves = generateKnightMoves(square, i);
        bitboard reachable = 0;
        for(int i = 0; i < moves.moveCount; i++) {
            reachable |= moves.moves[i].toSquare;
        }
        knightTable[i].moves = std::make_unique<Moves>();
        knightTable[i].moves->copy(moves);
        knightTable[i].reachable = reachable;
        knightTable[i].isSet = true;
    }
}


/*
 * Returns all moves that a knight could do from the given index. Move should still be check afterwards if they attempt to capture a own piece
 */
void MagicBitboards::appendKnightMoves(int index, Moves &moves, bitboard collisions) {
    moves.appendMovesWithoutCollision(*knightTable[index].moves, collisions);
}

/*
 * Returns all square a knight could potentiall reach from a given square
 */
bitboard MagicBitboards::getKnightReachableSquares(int index) {
    return knightTable[index].reachable;
}

/*
 * Initialises the king table. The king table does not store castling
 */
void MagicBitboards::initKingTable() {
    for(int j = 0; j < 64; j++) {
        kingTable[j].moves = std::make_unique<Moves>();
        bitboard square = ((bitboard) 1) << j;
        bitboard reachable = 0;
        int x = j % 8;
        int y = j / 8;
        for(int i = 0; i < 8; i++) {
            //Going over directions
            int vx = 1 * (i == 0 || i == 3 || i == 4) - 1 * (i == 1 || i == 2 || i == 6); ;
            int vy = 1 * (i == 2 || i == 0 || i == 7) - 1 * (i == 3 || i == 1 || i == 5);
            int tx = x + vx;
            int ty = y + vy;
            bitboard targetSquare = 1;
            targetSquare = targetSquare << (tx + 8 * ty);
            if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8){
                Move m;
                m.fromSquare = square;
                m.toSquare = targetSquare;
                m.startingPiece = KING;
                m.endingPiece = KING;
                kingTable[j].moves->moves[kingTable[j].moves->moveCount++] = m;
                reachable |= targetSquare;
            }
        }
        kingTable[j].reachable = reachable;
    }
}


/*
 * Returns all moves that a king could do from the given index except castling. Move should still be check afterwards if they attempt to capture a own piece
 */
void MagicBitboards::appendKingMoves(int index, Moves &moves, bitboard collisions) {
    moves.appendMovesWithoutCollision(*kingTable[index].moves, collisions);
}

/*
 * Returns all square a king could potentially reach from a given square excluding castling
 */
bitboard MagicBitboards::getKingReachableSquares(int index) {
    return kingTable[index].reachable;
}

/*
 * Returns a ptr to an entry from the lookup table, usefull for fast looksups without copying
 */
const Entry *MagicBitboards::getBishopEntry(bitboard hitmap, int index) {
    bitboard blockers = hitmap & bishopTable[index].blockerOverlay;
    uint64_t key = (blockers * bishopTable[index].magicNumber) >> bishopTable[index].indexShift;
    return &bishopTable[index].entries[key];
}

/*
 * Returns a ptr to an entry from the lookup table, usefull for fast looksups without copying
 */
const Entry *MagicBitboards::getRookEntry(bitboard hitmap, int index) {
    bitboard blockers = hitmap & rookTable[index].blockerOverlay;
    uint64_t key = (blockers * rookTable[index].magicNumber) >> rookTable[index].indexShift;
    return &rookTable[index].entries[key];
}

/*
 * Returns all square a king could potentially reach from a given square excluding castling
 */
const Entry *MagicBitboards::getKingEntry(int index) {
    return &kingTable[index];
}

/*
 * Returns all square a king could potentially reach from a given square excluding castling
 */
const Entry *MagicBitboards::getKnightEntry(int index) {
    return &knightTable[index];
}