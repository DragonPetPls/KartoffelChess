#include <iostream>

#include "Communication/Communication.h"
#include "Communication/Writer.h"
#include "Game/Game.h"
#include "Tests/Test.h"
#include "Tuner/Tuner.h"

int main() {

    std::cout << "Kartoffeln" << std::endl;
    Writer::allowDefault();
    Communication c;
    c.startCommunication();

    return 0;
}
