//
// Created by fabian on 11/19/24.
//

#ifndef SEARCH_H
#define SEARCH_H
#include <atomic>
#include <unordered_map>

#include "../Game/Game.h"

constexpr int WIN = 100000;

constexpr int EXACT = 0;
constexpr int UPPERBOUND = 1;
constexpr int LOWERBOUND = 2;

//Parameters
constexpr int NULL_MOVE_DEPTH_REDUCTION = 5; //How much we reduce depth when checking a null move
constexpr double NULL_MOVE_FACTOR = 0.8; //What percentage of depths perform late move reduction

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

    int quiescence(Game &g, int alpha, int beta, int depth, int maxDepth);
public:
    void ponder(Game g, const std::atomic<bool> &stop);
    void search(Game &g, const std::atomic<bool> &stop);
    void searchToDepth(Game &g, int toDepth);
    int negamax(Game &g, int alpha, int beta, int depth, int maxDepth, Moves& killerMoves, bool isPVNode);

    void printPrincipleVariation(Game &g);

    void clearTranspositionTable() {
        transpositionTable.clear();
    }

    const Node* getNodeFromTable(Game &g) const {
        if(transpositionTable.find(g.key()) == transpositionTable.end()) {
            return nullptr;
        } else {
            return &transpositionTable.at(g.key());
        }
    }
};



#endif //SEARCH_H
