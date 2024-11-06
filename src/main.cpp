#include <iostream>
#include "Game.h"
#include "Test.h"

#define TEST

int main() {


#ifdef TEST
    Test::testDoMove();
#endif
    return 0;
}
