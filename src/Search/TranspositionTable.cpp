//
// Created by fabian on 3/18/25.
//

#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
}

/**
 * Checks the table for an entry
 * @return a copy of the entry
 */
Node TranspositionTable::lookup(const Game& game, bool &exists) {
	mtx.lock();
	if(transpositionTable.find(game.key()) == transpositionTable.end()) {
		exists = false;
		mtx.unlock();
		return {};
	} else {
		exists = true;
		mtx.unlock();
		return transpositionTable.at(game.key());
	}
}

/**
 * Writes inside the table.
 * @param game acts as the key
 * @param value new value
 * @param bestMoveIndex new bestMoveIndex
 * @param flag new flag
 * @param depth new depth
 * @return true if writing was successful, false if the table is full
 */
bool TranspositionTable::write(const Game &game, int value, int bestMoveIndex, char flag, int depth) {
	mtx.lock();
	if(transpositionTable.size() >= maxTableEntries) {
		mtx.unlock();
		return false;
	}
	Node n{value, bestMoveIndex, flag, depth};
	if(transpositionTable.find(game.key()) == transpositionTable.end()) {
		transpositionTable.insert(std::make_pair(game.key(), n));
	} else {
		transpositionTable.at(game.key()) = n;
	}
	mtx.unlock();
	return true;
}

void TranspositionTable::clear() {
	mtx.lock();
	transpositionTable.clear();
	mtx.unlock();
}

