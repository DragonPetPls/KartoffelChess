//
// Created by fabia on 17.06.2025.
//

#ifndef MOVEORDERER_H
#define MOVEORDERER_H

#include "../Game/Game.h"

constexpr int SAFETY_DELTA_MARGIN = 200;

class MoveOrderer {
private:
    struct Order {
        int index;
        int value;
    };

    static const int MIDGAME_PIECE_VALUES[6];

    static int getMoveValue(const Move &move, const Game &g, Moves &killerMoves, int historyTable[6][64]);

public:
    static std::vector<int> rankMoves(const Game &g, const Moves &moves, int prevBestIndex, Moves &killerMoves, int historyTable[6][64]);
    static std::vector<int> rankCaptures(const Game &g, const Moves &moves, int delta);
};



#endif //MOVEORDERER_H
