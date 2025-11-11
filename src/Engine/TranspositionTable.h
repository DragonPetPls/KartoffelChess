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
	uint8_t bestMoveIndex;
	char flag;
	uint8_t depth;
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
	bool write(const Game& game, int value, uint8_t bestMoveIndex, char flag, uint8_t depth);
	void clear();
	uint32_t nodeCount = 0;
};

#endif //TRANSPOSITIONTABLE_H
