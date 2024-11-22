#include <iostream>

#include "Communication.h"
#include "Game.h"
#include "Test.h"

//#define TEST

int main() {

#ifndef TEST
    std::cout << "Kartoffeln" << std::endl;
    Communication c;
    c.startCommunication();
#endif

#ifdef TEST
    //Test::searchTest();
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
