//
// Transposition Table Implementation
//

#include "TranspositionTable.h"
#include <algorithm>

TranspositionTable::TranspositionTable(size_t sizeMB) {
    resize(sizeMB);
}

void TranspositionTable::resize(size_t sizeMB) {
    size_t numBytes = sizeMB * 1024 * 1024;
    size_t numEntries = numBytes / sizeof(TTEntry);

    capacity = 1;
    while ((capacity << 1) <= numEntries) {
        capacity <<= 1;
    }
    mask = capacity - 1;
    table.assign(capacity, TTEntry{});
}

int TranspositionTable::scoreToTT(int score, int ply) {
    if (score >= IS_MATE_SCORE) return score + ply;
    if (score <= -IS_MATE_SCORE) return score - ply;
    return score;
}

int TranspositionTable::scoreFromTT(int score, int ply) {
    if (score >= IS_MATE_SCORE) return score - ply;
    if (score <= -IS_MATE_SCORE) return score + ply;
    return score;
}

bool TranspositionTable::probe(uint64_t key, int ply, int depth, int alpha, int beta, int &score, Move &bestMove) const {
    if (capacity == 0) return false;
    const TTEntry &entry = table[key & mask];

    if (entry.key == key) {
        bestMove = entry.bestMove;
        if (entry.depth >= depth) {
            int ttScore = scoreFromTT(entry.value, ply);
            if (entry.flag == EXACT) {
                score = ttScore;
                return true;
            }
            if (entry.flag == LOWERBOUND && ttScore >= beta) {
                score = ttScore;
                return true;
            }
            if (entry.flag == UPPERBOUND && ttScore <= alpha) {
                score = ttScore;
                return true;
            }
        }
    }
    return false;
}

bool TranspositionTable::probeMove(uint64_t key, Move &bestMove) const {
    if (capacity == 0) return false;
    const TTEntry &entry = table[key & mask];
    if (entry.key == key) {
        bestMove = entry.bestMove;
        return true;
    }
    return false;
}

void TranspositionTable::store(uint64_t key, int score, const Move &bestMove, uint8_t flag, uint8_t depth, int ply) {
    if (capacity == 0) return;
    TTEntry &entry = table[key & mask];

    if (entry.key == 0 || entry.key == key || depth >= entry.depth || flag == EXACT) {
        entry.key = key;
        entry.value = static_cast<int16_t>(scoreToTT(score, ply));
        if (bestMove.fromSquare != 0 || bestMove.toSquare != 0) {
            entry.bestMove = bestMove;
        }
        entry.flag = flag;
        entry.depth = depth;
    }
}

void TranspositionTable::clear() {
    std::fill(table.begin(), table.end(), TTEntry{});
}

uint32_t TranspositionTable::getEntryCount() const {
    return static_cast<uint32_t>(capacity);
}
