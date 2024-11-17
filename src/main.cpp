#include <iostream>
#include "Game.h"
#include "Test.h"

#define TEST

int main() {



#ifdef TEST
    //Test::testDoMove();
    //Test::testMoveGen();
    //Test::testFen();
    Test::perft();
    //Test::consolPerft();
    Test::statusPerft();
    //Test::zobristPerft();
#endif
    return 0;
}
