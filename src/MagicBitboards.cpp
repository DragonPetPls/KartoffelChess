//
// Created by fabian on 11/13/24.
//

#include "MagicBitboards.h"

#include <iostream>
#include <cmath>
#include <random>

#include "magicConstants.h"

MagicTableSquare MagicBitboards::bishopTable[64];
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
            for(auto move : moves) {
                reachable |= move.toSquare;
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
std::vector<Move> MagicBitboards::generateBishopMoves(bitboard square, int index, const bitboard &hitmap) {
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
    return moves;
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
            for(auto move : moves) {
                reachable |= move.toSquare;
            }

            if(bishopTable[i].entries[key].isSet == false) {
                bishopTable[i].entries[key].isSet = true;
                bishopTable[i].entries[key].reachable = reachable;
                bishopTable[i].entries[key].moves = moves;
            } else if (bishopTable[i].entries[key].reachable != reachable) {
                std::cout << "Error in MagicBitboards::initBishopTable()" << std::endl;
            }
        }
    }
}

/*
 * Returns all bishop moves from the lookup table
 */
std::vector<Move> MagicBitboards::getBishopMoves(bitboard hitmap, int index) {

    bitboard blockers = hitmap & bishopTable[index].blockerOverlay;
    uint64_t key = (blockers * bishopTable[index].magicNumber) >> bishopTable[index].indexShift;
    return bishopTable[index].entries[key].moves;
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
 * Returns a mask of all relevant square for blockers
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
        isInit = true;
    }
}
