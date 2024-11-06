//
// Created by fabian on 11/5/24.
//

#include <string>
#include <sstream>
#include <iostream>
#include "Test.h"
#include "Game.h"

void Test::testPrintGame() {
    Game g;
    g.loadStartingPosition();
    g.printGame();
}

void Test::testDoMove() {
    Game g;
    g.loadStartingPosition();

    std::string gameStr = "d2d4 g8f6 c2c4 e7e6 g1f3 d7d5 b1c3 d5c4 e2e4 f8b4 c1g5 c7c5 f1c4 c5d4 f3d4 c8d7 e1g1 b8c6 a1c1 a7a6 a2a3 b4c3 b2c3 e8g8 h2h3 c6e5 c4b3 e5g6 e4e5 g6e5 f2f4 e5c6 b3c4 h7h6 g5h4 d8e7 d1b3 c6a5 b3b4 e7b4 c3b4 a5c4 c1c4 f6d5 c4c1 a8c8 c1c5 b7b6 c5c8 f8c8 f1f3 c8c1 g1h2 c1d1 h4f2 d5f6 d4b3 f6d5 b3d4 d5f6 d4b3 d7a4 b3d4 f6e4 f2e3 d1d3 e3g1 d3d1 g1e3 d1d3 e3g1 d3d2 f3f1 b6b5 f1e1 e4f2 d4f3 d2b2 f3e5 b2d2 e1c1 f7f6 e5f3 d2b2 g1f2 b2f2 c1c6 a4b3 h2g3 f2c2 c6a6 b3d5 a6b6 d5c6 b6b8 g8h7 h3h4 c2c3 b8c8 h7g6 g3h2 c3c4 c8c7 c6f3 c7c4 b5c4 g2f3 c4c3 h2g3 c3c2 b4b5 c2c1q a3a4 c1g1 g3h3 g6f5 b5b6 f5f4 b6b7 g1g3";

    //gameStr = "d2d4 h7h5 d4d5 e7e5 d5e6 h5h4 g2g4 h4g3 e6d7 e8e7 b2b4 c7c5 b4b5 b8c6 b5c6";

    std::vector<std::string> moveStr;
    std::stringstream ss(gameStr);
    std::string ms;

    while(ss >> ms){
        moveStr.push_back(ms);
    }

    for(int i = 0; i < moveStr.size(); i++){
        g.doMoveAsString(moveStr[i]);
        std::cout << moveStr[i] << std::endl;
        g.printGame();
    }

    //Lot of en passant cases here
    g.loadStartingPosition();
    gameStr = "d2d4 h7h5 d4d5 e7e5 d5e6 h5h4 g2g4 h4g3 e6d7 e8e7 b2b4 c7c5 b4b5 b8c6 b5c6";

    ss.str(gameStr);

    while(ss >> ms){
        moveStr.push_back(ms);
    }

    for(int i = 0; i < moveStr.size(); i++){
        g.doMoveAsString(moveStr[i]);
        std::cout << moveStr[i] << std::endl;
        g.printGame();
    }

}
