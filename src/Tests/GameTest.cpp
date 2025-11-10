//
// Created by fabian on 10.11.25.
//

#include <gtest/gtest.h>

#include "../Game/Game.h"
#include "Test.h"

uint64_t perft(Game &g, int depth, bool printInfo = false) {

    //g.printGame();
    if (depth == 0) {
        return 1;
    }

    auto moves = g.getAllPseudoLegalMoves();
    int n = 0;
    for (int i = 0; i < moves.moveCount; i++) {
        g.doMove(moves[i]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }
        int p = perft(g, depth - 1);
        n += p;

        if (printInfo) {
            std::cout << Game::moveToString(moves[i]) << ": " << p << std::endl;
        }
        g.undoMove();
    }
    return n;
}

TEST(GameTests, Perft) {
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
        EXPECT_EQ(n, results[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

TEST(GameTests, StatusPerft) {
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
        ::Test::checkmates = 0;
        int n = ::Test::perftStatus(g, depth[i]);
        nodesSearched += n;
        if (n == results[i] && ::Test::checkmates == checkmatesResults[i]) {
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in test " << i << ". Result: " << n << " Correct: " << results[i] << " Checkmates: " <<
                    ::Test::checkmates << " Correct: " << checkmatesResults[i] << std::endl;
        }
        EXPECT_EQ(n, results[i]);
        EXPECT_EQ(::Test::checkmates, checkmatesResults[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

TEST(GameTests, ZobristPerft) {
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
        int n = ::Test::perftZobrist(g, depth[i]);
        nodesSearched += n;
        EXPECT_EQ(n, results[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}

TEST(GameTests, TranspositionPerft) {
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
    ::Test::transpositionTable.reserve(100000000);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        g.loadFen(fen[i]);
        ::Test::transpositionTable.clear();
        uint64_t n = ::Test::perftTransposition(g, depth[i]);
        nodesSearched += n;
        EXPECT_EQ(n, results[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / (elapsed.count() * 1000000) << " million nodes per second" << std::endl;
}