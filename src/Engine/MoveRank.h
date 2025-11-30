//
// Created by fabian on 30.11.25.
//

#ifndef KARTOFFELCHESS_MOVERANK_H
#define KARTOFFELCHESS_MOVERANK_H

#include "../Game/Game.h"

class MoveRank {
private:
    struct Order {
        int index;
        int value;
    };
    static int getMoveValue(const Move &move, const Game &g, Moves &killerMoves, double historyTable[6][64]);

public:
    static std::vector<int> rankMoves(const Game &g, const Moves &moves, int prevBestIndex, Moves &killerMoves, double historyTable[6][64], int& killerMoveCutoff);
    static std::vector<int> rankCaptures(const Game &g, const Moves &moves, int delta);
};


#endif //KARTOFFELCHESS_MOVERANK_H