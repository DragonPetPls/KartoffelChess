#include <iostream>

#include "Communication.h"
#include "Game.h"
#include "Test.h"

#define TEST

int main() {

    Communication c;
    c.startCommunication();

#ifdef TEST
    //Test::testDoMove();
    //Test::testMoveGen();
    //Test::testFen();
    //Test::perft();
    //Test::consolPerft();
    //Test::statusPerft();
    //Test::zobristPerft();
    //Test::zobristTest();
    //Test::transpositionPerft();
#endif
    return 0;
}
