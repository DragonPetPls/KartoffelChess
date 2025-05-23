#include <iostream>

#include "Communication.h"
#include "Game/Game.h"
#include "Tests/Test.h"

//#define TEST

int main() {

#ifndef TEST
    std::cout << "Kartoffeln" << std::endl;
    Communication c;
    c.startCommunication();
#endif

#ifdef TEST
    Test::searchTest();
    //Test::testDoMove();
    //Test::testMoveGen();
    //Test::testFen();
    //Test::perft();
    //Test::consolPerft();
    //Test::statusPerft();
    //Test::zobristPerft();
    //Test::zobristTest();
    //Test::transpositionPerft();
    //Test::captureTest();
#endif
    return 0;
}
