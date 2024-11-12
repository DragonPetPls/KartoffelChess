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
        std::cout << moveStr[i] << std::endl;
        g.doMoveAsString(moveStr[i]);
        g.printGame();
    }
    while(g.getGameHistoryCounter() != -1){
        std::cout << moveStr[g.getGameHistoryCounter()] <<std::endl;
        g.undoMove();
        g.printGame();
    }
}

void Test::testMoveGen() {
    Game g;
    g.loadStartingPosition();
    auto next = g.getAllPseudoLegalMoves();
    for(Move m: next){
        g.doMove(m);
        g.printGame();
        g.undoMove();
    }
}

void Test::testFen() {
    Game g;
    std::string fen[4] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
                        "rnbq1bnr/pppkpppp/8/3p4/4P3/8/PPPPKPPP/RNBQ1BNR w - - 2 3",
                            "8/8/8/8/8/5k2/2K5/8 w - - 0 1"};
    for(int i = 0; i < 4; i++) {
        std::cout << "===========================" << std::endl;
        std::cout << fen[i] << std::endl;
        g.loadFen(fen[i]);
        g.printGame();
    }
}

void Test::perft() {
    Game g;
    std::string fen[10] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
    "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",
        "K1k5/8/P7/8/8/8/8/8 w - - 0 1",
        "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",
        "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1"};

    int depth[10] = {5, 4, 5, 4, 4, 4, 6, 6, 4, 6};
    int results[10] = {4865609, 4085603, 674624, 422333, 2103487, 3894594, 217342, 2217, 1274206, 824064};
    for(int i = 0; i < 10; i++) {
        g.loadFen(fen[i]);
        int n = perft(g, depth[i]);
        if(n == results[i]) {
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in test " << i << ". Result: " << n << " Correct: " << results[i] << std::endl;
        }
    }
}

int Test::perft(Game &g, int depth, bool printInfo) {
    //g.printGame();
    if(depth == 0) {
        return 1;
    }

    auto next = g.getAllPseudoLegalMoves();
    int n = 0;
    for(Move m: next) {
        g.doMove(m);
        if(!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        int p = perft(g, depth - 1);
        n += p;

        if(printInfo) {
            std::cout << Game::moveToString(m) << ": " << p << std::endl;
        }
        g.undoMove();
    }
    return n;
}

void Test::consolPerft() {

    int depth = 0;
    std::string input;
    std::string fen;
    while(true) {
        Game g;
        getline(std::cin, input);
        if(input == "go" || input == "GO") {
            g.loadFen(fen);
            g.printGame();
            std::cout << perft(g, depth, true) << std::endl;
            continue;
        }
        if (input == "print") {
            g.printGame();
        }
        if(input.size() > 3) {
            fen = input;
            std::cout << "fen: " << fen << std::endl;
        } else {
            try {
                depth = stoi(input);
            } catch(...) {
                std::cout << "Invalid input." << std::endl;
            }
        }
    }
}
