//
// Created by fabian on 11/19/24.
//

#ifndef SEARCH_H
#define SEARCH_H
#include <atomic>
#include <unordered_map>

#define SEND_INFO

#include "TranspositionTable.h"
#include "../Game/Game.h"

constexpr int WIN = 100000;

constexpr int EXACT = 0;
constexpr int UPPERBOUND = 1;
constexpr int LOWERBOUND = 2;

//Parameters
constexpr int NULL_MOVE_DEPTH_REDUCTION = 5; //How much we reduce depth when checking a null move
constexpr double NULL_MOVE_FACTOR = 0.8; //What percentage of depths perform late move reduction

class Search {

private:
    TranspositionTable table;
    const std::atomic<bool>* stop = nullptr;
    int historyTable[6][64] = {};

public:
    int quiescence(Game &g, int alpha, int beta, int depth, int maxDepth);
    void ponder(Game g, const std::atomic<bool> &stop);
    void search(Game &g, const std::atomic<bool> &stop);
    void searchToDepth(Game &g, int toDepth);
    int negamax(Game &g, int alpha, int beta, int depth, int maxDepth, Moves& killerMoves, bool isPVNode);

    void printPrincipleVariation(Game &g);

    void clearTranspositionTable() {
        table.clear();
    }

    Node getNodeFromTable(Game &g) {
        bool exists;
        return table.lookup(g, exists);
    }
};



#endif //SEARCH_H
