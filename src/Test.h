//
// Created by fabian on 11/5/24.
//

#ifndef SRC_TEST_H
#define SRC_TEST_H
#include "Game.h"

/*
 * A collection of tests
 */
class Test {
private:
    static int perft(Game &g, int depth, bool printInfo = false);
public:
    static void testPrintGame();
    static void testDoMove();
    static void testMoveGen();
    static void testFen();
    static void perft();
    static void consolPerft();
};


#endif //SRC_TEST_H
