//
// Created by fabian on 3/18/25.
//

#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H
#include <mutex>
#include <unordered_map>
#include "../Game/Game.h"

struct Node {
	int value;
	int bestMoveIndex;
	char flag;
	int depth;
};

class TranspositionTable {
private:
	std::mutex mtx;
	int maxTableSizeInBytes = 1024 * 1024 * 1024;
	int entrySize = sizeof(Node) + sizeof(GameKey);
	int maxTableEntries = maxTableSizeInBytes / entrySize;
	std::unordered_map<GameKey, Node> transpositionTable;
public:
	TranspositionTable();
	Node lookup(const Game& key, bool &exists);
	bool write(const Game& game, int value, int bestMoveIndex, char flag, int depth);
	void clear();
};

#endif //TRANSPOSITIONTABLE_H
