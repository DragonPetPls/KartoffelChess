#include <iostream>

#include "Communication.h"
#include "Game/Game.h"
#include "Tests/Test.h"
#include "Tuner/Tuner.h"

//#define TEST
//#define TUNE

int main() {

#ifndef TUNE
#ifndef TEST
    std::cout << "Kartoffeln" << std::endl;
    Communication c;
    c.startCommunication();
#endif
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

#ifdef TUNE
    Tuner::tuneParameter();
    //Tuner::generateData();
    //Tuner::readData();
#endif

    return 0;
}
