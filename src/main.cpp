#include <iostream>
#include "Game.h"
#include "Test.h"

#define TEST

int main() {

//MagicBitboards::generateBishopMagicNumbers();

#ifdef TEST
    //Test::testDoMove();
    //Test::testMoveGen();
    //Test::testFen();
    Test::perft();
    //Test::consolPerft();
    //Test::statusPerft();
#endif
    return 0;
}
