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

struct Node {
    int value;
    int bestMoveIndex;
};

class Search {

private:
    std::unordered_map<GameKey, Node> transpositionTable;
    const std::atomic<bool>* stop = nullptr;

public:

    void search(Game &g, const std::atomic<bool> &stop);
    int negamax(Game &g, int alpha, int beta, int depth);

    const Node* getNodeFromTable(Game &g) const {
        if(transpositionTable.find(g.key()) == transpositionTable.end()) {
            return nullptr;
        } else {
            return &transpositionTable.at(g.key());
        }
    }
};



#endif //SEARCH_H
