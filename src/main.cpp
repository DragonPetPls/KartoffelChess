#include <iostream>

#include "Communication.h"
#include "Game/Game.h"
#include "KITest/KiTrainer.h"
#include "Tests/Test.h"

#define TEST
//#define KI_TRAINING

int main() {
#ifndef TEST
#ifndef KI_TRAINING
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

#ifdef KI_TRAINING
    KiTrainer::train();
#endif

    return 0;
}
