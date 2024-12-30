//
// Created by fabian on 11/5/24.
//

#include <string>
#include <sstream>
#include <iostream>
#include "Test.h"

#include <bits/chrono.h>

#include "Evaluation.h"
#include "Game.h"
#include "Search.h"

int Test::checkmates;
std::unordered_map<GameKey, testNode> Test::transpositionTable;

void Test::testPrintGame() {
    Game g;
    g.loadStartingPosition();
    g.printGame();
}

void Test::testDoMove() {
    Game g;
    g.loadStartingPosition();

    std::string gameStr =
            "d2d4 g8f6 c2c4 e7e6 g1f3 d7d5 b1c3 d5c4 e2e4 f8b4 c1g5 c7c5 f1c4 c5d4 f3d4 c8d7 e1g1 b8c6 a1c1 a7a6 a2a3 b4c3 b2c3 e8g8 h2h3 c6e5 c4b3 e5g6 e4e5 g6e5 f2f4 e5c6 b3c4 h7h6 g5h4 d8e7 d1b3 c6a5 b3b4 e7b4 c3b4 a5c4 c1c4 f6d5 c4c1 a8c8 c1c5 b7b6 c5c8 f8c8 f1f3 c8c1 g1h2 c1d1 h4f2 d5f6 d4b3 f6d5 b3d4 d5f6 d4b3 d7a4 b3d4 f6e4 f2e3 d1d3 e3g1 d3d1 g1e3 d1d3 e3g1 d3d2 f3f1 b6b5 f1e1 e4f2 d4f3 d2b2 f3e5 b2d2 e1c1 f7f6 e5f3 d2b2 g1f2 b2f2 c1c6 a4b3 h2g3 f2c2 c6a6 b3d5 a6b6 d5c6 b6b8 g8h7 h3h4 c2c3 b8c8 h7g6 g3h2 c3c4 c8c7 c6f3 c7c4 b5c4 g2f3 c4c3 h2g3 c3c2 b4b5 c2c1q a3a4 c1g1 g3h3 g6f5 b5b6 f5f4 b6b7 g1g3";

    //gameStr = "d2d4 h7h5 d4d5 e7e5 d5e6 h5h4 g2g4 h4g3 e6d7 e8e7 b2b4 c7c5 b4b5 b8c6 b5c6";

    std::vector<std::string> moveStr;
    std::stringstream ss(gameStr);
    std::string ms;

    while (ss >> ms) {
        moveStr.push_back(ms);
    }

    for (int i = 0; i < moveStr.size(); i++) {
        std::cout << moveStr[i] << std::endl;
        g.doMoveAsString(moveStr[i]);
        g.printGame();
    }
    while (g.getGameHistoryCounter() != -1) {
        std::cout << moveStr[g.getGameHistoryCounter()] << std::endl;
        g.undoMove();
        g.printGame();
    }
}

void Test::testMoveGen() {
    Game g;
    g.loadStartingPosition();
    auto next = g.getAllPseudoLegalMoves();
    for (int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        g.printGame();
        g.undoMove();
    }
}

void Test::testFen() {
    Game g;
    std::string fen[4] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbq1bnr/pppkpppp/8/3p4/4P3/8/PPPPKPPP/RNBQ1BNR w - - 2 3",
        "8/8/8/8/8/5k2/2K5/8 w - - 0 1"
    };
    for (int i = 0; i < 4; i++) {
        std::cout << "===========================" << std::endl;
        std::cout << fen[i] << std::endl;
        g.loadFen(fen[i]);
        g.printGame();
    }
}

void Test::perft() {
    Game g;
    std::string fen[10] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",
        "K1k5/8/P7/8/8/8/8/8 w - - 0 1",
        "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",
        "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1"
    };

    int depth[10] = {5, 4, 5, 4, 4, 4, 6, 6, 4, 6};
    int results[10] = {4865609, 4085603, 674624, 422333, 2103487, 3894594, 217342, 2217, 1274206, 824064};
    int nodesSearched = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        g.loadFen(fen[i]);
        int n = perft(g, depth[i]);
        nodesSearched += n;
        if (n == results[i]) {
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in test " << i << ". Result: " << n << " Correct: " << results[i] << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

uint64_t Test::perft(Game &g, int depth, bool printInfo) {

    //g.printGame();
    if (depth == 0) {
        return 1;
    }

    auto next = g.getAllPseudoLegalMoves();
    int n = 0;
    for (int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        int p = perft(g, depth - 1);
        n += p;

        if (printInfo) {
            std::cout << Game::moveToString(next.moves[i]) << ": " << p << std::endl;
        }
        g.undoMove();
    }
    return n;
}

void Test::consolPerft() {
    int depth = 0;
    std::string input;
    std::string fen;
    while (true) {
        Game g;
        getline(std::cin, input);
        if (input == "go" || input == "GO") {
            if (fen == "") {
                g.loadStartingPosition();
            } else {
                g.loadFen(fen);
            }
            g.printGame();
            uint64_t nodesSearched = 0;
            auto start = std::chrono::high_resolution_clock::now();
            nodesSearched = perft(g, depth, true);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
            std::cout << nodesSearched << " nodes were searched." << std::endl;
            std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
            continue;
        }
        if (input == "print") {
            g.printGame();
        }
        if (input.size() > 3) {
            fen = input;
            std::cout << "fen: " << fen << std::endl;
        } else {
            if (input.size() == 1) {
                depth = input[0] - '0';
                std::cout << "depth: " << depth << std::endl;
            }
        }
    }
}

void Test::statusPerft() {
    Game g;
    std::string fen[4] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 "
    };

    int depth[4] = {5, 4, 6, 5};
    int results[4] = {4865609, 4085603, 11030083, 15833292};
    int checkmatesResults[4] = {347, 43, 2733, 50562};
    int nodesSearched = 0;
    std::cout << "Starting test" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 4; i++) {
        g.loadFen(fen[i]);
        checkmates = 0;
        int n = perftStatus(g, depth[i]);
        nodesSearched += n;
        if (n == results[i] && checkmates == checkmatesResults[i]) {
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in test " << i << ". Result: " << n << " Correct: " << results[i] << " Checkmates: " <<
                    checkmates << " Correct: " << checkmatesResults[i] << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

int Test::perftStatus(Game &g, int depth, bool printInfo) {
    //g.printGame();
    char status = g.getStatus();
    if (status == WHITE_WON || status == BLACK_WON) {
        checkmates += depth == 0;
    }

    if (depth == 0) {
        return 1;
    }

    auto next = g.getAllPseudoLegalMoves();
    int n = 0;
    for (int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        int p = perftStatus(g, depth - 1);
        n += p;

        if (printInfo) {
            std::cout << Game::moveToString(next.moves[i]) << ": " << p << std::endl;
        }
        g.undoMove();
    }
    return n;
}

void Test::zobristPerft() {
    Game g;
    std::string fen[10] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",
        "K1k5/8/P7/8/8/8/8/8 w - - 0 1",
        "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",
        "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1"
    };

    int depth[10] = {5, 4, 5, 4, 4, 4, 6, 6, 4, 6};
    int results[10] = {4865609, 4085603, 674624, 422333, 2103487, 3894594, 217342, 2217, 1274206, 824064};
    int nodesSearched = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        g.loadFen(fen[i]);
        int n = perftZobrist(g, depth[i]);
        nodesSearched += n;
        if (n == results[i]) {
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in test " << i << ". Result: " << n << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

int Test::perftZobrist(Game &g, int depth, bool printInfo) {
    //g.printGame();
    if (depth == 0) {
        return 1;
    }

    auto next = g.getAllPseudoLegalMoves();
    int n = 0;
    for (int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }

        //checking if the hashing worked:
        uint64_t hash = g.hashValue;
        g.setHashValue();
        if (hash != g.hashValue) {
            std::cout << "error: move hash: " << hash << " calc hash: " << g.hashValue << std::endl;
            g.printGame();
            std::cout << Game::moveToString(next.moves[i]) << std::endl;
            g.undoMove();
            std::cout << "old hash: " << g.hashValue << std::endl;
            g.doMove(next.moves[i]);
            std::cout << "re hash: " << g.hashValue << std::endl;
        }

        int p = perftZobrist(g, depth - 1);
        n += p;

        if (printInfo) {
            std::cout << Game::moveToString(next.moves[i]) << ": " << p << std::endl;
        }
        g.undoMove();
    }
    return n;
}

void Test::zobristTest() {

    Game g;
    g.loadStartingPosition();
    g.doMoveAsString("e2e4");
    g.doMoveAsString("d7d5");
    g.doMoveAsString("g1f3");
    g.printGame();
    std::cout << g.hashValue << std::endl;

    g.loadStartingPosition();
    g.doMoveAsString("e2e3");
    g.doMoveAsString("d7d5");
    g.doMoveAsString("e3e4");
    g.doMoveAsString("d8d6");
    g.doMoveAsString("d1e2");
    g.doMoveAsString("d6d7");
    g.doMoveAsString("e2d1");
    g.doMoveAsString("d7d8");
    g.doMoveAsString("g1f3");
    g.printGame();
    std::cout << g.hashValue << std::endl;

    std::cout << "==============" << std::endl;

    g.loadStartingPosition();
    std::cout << g.hashValue << std::endl;
    g.doMoveAsString("b1c3");
    g.doMoveAsString("b8b6");
    g.doMoveAsString("c3b1");
    g.doMoveAsString("b6b8");
    g.printGame();
    std::cout << g.hashValue << std::endl;
}

void Test::transpositionPerft() {
    Game g;
    std::string fen[10] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",
        "K1k5/8/P7/8/8/8/8/8 w - - 0 1",
        "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",
        "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1"
    };

    int depth[10] = {5, 4, 5, 4, 4, 4, 6, 6, 4, 6};
    int results[10] = {4865609, 4085603, 674624, 422333, 2103487, 3894594, 217342, 2217, 1274206, 824064};
    int nodesSearched = 0;
    transpositionTable.reserve(100000000);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        g.loadFen(fen[i]);
        transpositionTable.clear();
        uint64_t n = perftTransposition(g, depth[i]);
        nodesSearched += n;
        if (n == results[i]) {
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in test " << i << ". Result: " << n << " Correct: " << results[i] << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

uint64_t Test::perftTransposition(Game &g, int depth, bool printInfo) {
    //g.printGame();
    if (depth == 0) {
        return 1;
    }

    bool newEntry;
    if(transpositionTable.find(g.key()) != transpositionTable.end()) {
        if(transpositionTable[g.key()].depth == depth) {
            return transpositionTable[g.key()].nodes;
        }
        newEntry = false;
    } else {
        newEntry = true;
    }

    auto next = g.getAllPseudoLegalMoves();
    uint64_t n = 0;
    for (int i = 0; i < next.moveCount; i++) {
        g.doMove(next.moves[i]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }

        uint64_t p = perftTransposition(g, depth - 1);
        n += p;

        if (printInfo) {
            std::cout << Game::moveToString(next.moves[i]) << ": " << p << std::endl;
        }
        g.undoMove();
    }
    if(newEntry) {
        transpositionTable.insert(std::make_pair(g.key(), testNode{n, depth}));
    }

    return n;
}

/*
 * Testing the search function
 */
void Test::searchTest() {
    std::string fen[24] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",
        "K1k5/8/P7/8/8/8/8/8 w - - 0 1",
        "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",
        "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1",
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2",
        "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R b KQkq - 3 3",
        "r1bqk2r/pppp1ppp/2n2n2/2b1p3/2B1P3/2N2N2/PPPP1PPP/R1BQ1RK1 b kq - 7 5",
        "r1bq1rk1/1ppp1pp1/p1n2n1p/2b1p3/2B1P2B/2NP1N2/PPP2PPP/R2Q1RK1 b - - 1 8",
        "r1bq1rk1/1pp1bpp1/p1np1B1p/4p3/P1B1P3/2NP1N2/1PP2PPP/R2Q1RK1 b - - 0 10",
        "r1bq1rk1/1pp2pb1/p1np2pp/3Np3/P1B1P3/3P1N2/1PP2PPP/R2QR1K1 w - - 2 13",
        "r1b2rk1/1pp1qpb1/p2p2p1/4p2p/P1B1P3/2PP1N1P/1P3PP1/R2QR1K1 w - - 0 16",
        "r1b2rk1/1pp1qpb1/3p2p1/p3p1Np/P1B1P3/R1PP3P/1P3PP1/3QR1K1 b - - 1 17",
        "r1b2r2/1pp1qpbk/3p2p1/p3p2p/P1B1P3/R1PP1R1P/1P3PP1/3Q2K1 w - - 4 20",
        "5r2/1pp1q1bk/3p2p1/r3pb1p/P1B5/2PP1R1P/3R1PP1/3Q2K1 b - - 1 24",
        "4rr2/1p1bq1bk/2p3p1/3pp2p/P7/2PPR2P/4RPP1/1B2Q1K1 b - - 5 29",
        "8/1p4b1/2p2kp1/3pp3/P5K1/2PP4/5PP1/1B6 b - - 0 40",
        "8/1p6/2p2kp1/3pp3/P1P2bK1/3P4/5PP1/1B6 w - - 1 42",
        "8/1p6/2p2kp1/3pp3/P1P2bK1/3P4/5PP1/1B6 w - - 1 42"};
    int depth = 8;
    Game g;
    Search s;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 24; i++) {
        g.loadFen(fen[i]);
        s.searchToDepth(g, depth);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << Evaluation::evaluationCount << " evaluations performed" << std::endl;
    std::cout << elapsed.count() << " seconds" << std::endl;
}

void Test::captureTest() {
    int depth = 0;
    std::string input;
    std::string fen;
    while (true) {
        Game g;
        getline(std::cin, input);
        if (input == "go" || input == "GO") {
            if (fen == "") {
                g.loadStartingPosition();
            } else {
                g.loadFen(fen);
            }
            g.printGame();
            auto next = g.getAllPseudoLegalCaptures();
            for(int i = 0; i < next.moveCount; i++) {
                std::cout << Game::moveToString(next.moves[i]) << " ";
            }
            std::cout << std::endl;
            continue;
        }
        if (input == "print") {
            g.printGame();
        }
        if (input.size() > 3) {
            fen = input;
            std::cout << "fen: " << fen << std::endl;
        } else {
            if (input.size() == 1) {
                depth = input[0] - '0';
                std::cout << "depth: " << depth << std::endl;
            }
        }
    }
}
