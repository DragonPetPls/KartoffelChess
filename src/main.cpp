#include <iostream>
#include "Game.h"
#include "Test.h"

#define TEST

int main() {


#ifdef TEST
    Test::testPrintGame();
    std::cout << "print test completed" << std::endl;
    Test::testDoMove();
#endif
    return 0;
}
