//
// Search Header Definition
//

#ifndef SEARCH_H
#define SEARCH_H

#include <atomic>
#include <cmath>
#include <algorithm>
#include "TranspositionTable.h"
#include "../Game/Game.h"

constexpr int WIN = 100000;
constexpr int MAX_PLY = 128;

class Search {
private:
    TranspositionTable table;
    const std::atomic<bool>* stop = nullptr;

    Move killerMoves[MAX_PLY][2] = {};
    int historyTable[2][64][64] = {};
    int LMRTable[64][64] = {};
    Move bestRootMove = nullMove;
    Move lastCompletedBestMove = nullMove;

    void initLMR();
    int quiescence(Game &g, int alpha, int beta, int depth, int ply);

public:
    Search();

    void ponder(Game g, const std::atomic<bool> &stop);
    void search(Game &g, const std::atomic<bool> &stop);
    void searchToDepth(Game &g, int toDepth);
    int negamax(Game &g, int alpha, int beta, int depth, int ply, bool isPVNode);

    void printPrincipleVariation(Game &g);

    void clearTranspositionTable() {
        table.clear();
        lastCompletedBestMove = nullMove;
        bestRootMove = nullMove;
    }

    Move getBestMove() const {
        if (lastCompletedBestMove.fromSquare != 0 || lastCompletedBestMove.toSquare != 0) {
            return lastCompletedBestMove;
        }
        return bestRootMove;
    }

    bool probeTTMove(uint64_t key, Move &m) const {
        return table.probeMove(key, m);
    }

    uint32_t getNodeCount() const {
        return table.getEntryCount();
    }
};

#endif //SEARCH_H
