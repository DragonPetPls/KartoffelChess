//
// Transposition Table Implementation
//

#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include <vector>
#include <cstdint>
#include "../Game/Game.h"

constexpr int EXACT = 0;
constexpr int UPPERBOUND = 1;
constexpr int LOWERBOUND = 2;

constexpr int IS_MATE_SCORE = 80000;
constexpr int MATE_VALUE = 100000;

struct TTEntry {
    uint64_t key = 0;
    int16_t value = 0;
    uint8_t flag = 0;
    uint8_t depth = 0;
    Move bestMove = nullMove;
};

class TranspositionTable {
private:
    std::vector<TTEntry> table;
    size_t capacity = 0;
    size_t mask = 0;

public:
    TranspositionTable(size_t sizeMB = 32);
    void resize(size_t sizeMB);

    static int scoreToTT(int score, int ply);
    static int scoreFromTT(int score, int ply);

    bool probe(uint64_t key, int ply, int depth, int alpha, int beta, int &score, Move &bestMove) const;
    bool probeMove(uint64_t key, Move &bestMove) const;
    void store(uint64_t key, int score, const Move &bestMove, uint8_t flag, uint8_t depth, int ply);
    void clear();

    uint32_t getEntryCount() const;
};

#endif //TRANSPOSITIONTABLE_H
