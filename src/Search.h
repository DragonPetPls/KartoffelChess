//
// Created by fabian on 11/19/24.
//

#ifndef SEARCH_H
#define SEARCH_H
#include <atomic>
#include <unordered_map>

#include "Game.h"

constexpr int INF = 1000000;
constexpr int WIN = 100000;

constexpr int EXACT = 0;
constexpr int UPPERBOUND = 1;
constexpr int LOWERBOUND = 2;

struct Node {
    int value;
    int bestMoveIndex;
    char flag;
    int depth;
};

class Search {

private:
    std::unordered_map<GameKey, Node> transpositionTable;
    const std::atomic<bool>* stop = nullptr;
    int historyTable[6][64] = {};

public:

    void search(Game &g, const std::atomic<bool> &stop);
    void searchToDepth(Game &g, int toDepth);
    int negamax(Game &g, int alpha, int beta, int depth, int maxDepth, Moves& killerMoves);

    const Node* getNodeFromTable(Game &g) const {
        if(transpositionTable.find(g.key()) == transpositionTable.end()) {
            return nullptr;
        } else {
            return &transpositionTable.at(g.key());
        }
    }
};



#endif //SEARCH_H
