#include <iostream>

#include "Communication.h"
#include "Game.h"
#include "Test.h"
#include "Trainer.h"

//#define TEST
#define TRAINER

int main() {

#ifndef TEST
#ifndef TRAINER
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

#ifdef TRAINER
    Trainer::train();
#endif
    return 0;
}
