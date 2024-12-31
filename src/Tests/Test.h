//
// Created by fabian on 11/5/24.
//

#ifndef SRC_TEST_H
#define SRC_TEST_H
#include <unordered_map>

#include "../Game/Game.h"

struct testNode {
    uint64_t nodes;
    int depth;
};

/*
 * A collection of tests
 */
class Test {
private:
    static int checkmates;
    static std::unordered_map<GameKey, testNode> transpositionTable;

    static uint64_t perft(Game &g, int depth, bool printInfo = false);
    static int perftStatus(Game &g, int depth, bool printInfo = false);
    static int perftZobrist(Game &g, int depth, bool printInfo = false);
    static uint64_t perftTransposition(Game &g, int depth, bool printInfo = false);
public:
    static void testPrintGame();
    static void testDoMove();
    static void testMoveGen();
    static void testFen();
    static void perft();
    static void consolPerft();
    static void statusPerft();
    static void zobristPerft();
    static void zobristTest();
    static void transpositionPerft();
    static void searchTest();
    static void captureTest();
};


#endif //SRC_TEST_H
