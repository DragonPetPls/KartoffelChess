//
// Created by fabian on 10.11.25.
//

#include <gtest/gtest.h>
#include "../Game/Game.h"
#include "../Engine/Search.h"
#include "../Engine/Evaluation.h"
#include "../Engine/Engine.h"

TEST(SearchTest, NodeCounting) {
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
    int depth = 10;
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

TEST(EngineTest, EasyPositions) {
    std::string fens[] = {
        "rnb1kbnr/pppppppp/8/4q3/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1",
        "rnb1k1nr/pppp1ppp/4pq2/2b5/4P3/2N5/PPPP1PPP/R1BQKBNR b KQkq - 1 1",
        "3Q4/5kp1/2q2P1p/p7/1p3K2/8/8/8 w - - 0 1"
    };
    std::string moves[] = {
        "f3e5",
        "f6f2",
        "d8e7"
    };
    Engine e;
    int counter = 0;
    for (auto fen : fens) {
        Game g;
        g.loadFen(fen);
        Move move = e.getMove(g, 10);
        std::string moveStr = Game::moveToString(move);
        EXPECT_EQ(moveStr, moves[counter]);
        counter++;
    }
}