//
// Created by fabian on 11/19/24.
//

#include "Engine.h"

#include <chrono>
#include <thread>

#include "Search.h"

/*
 * Returns the move the engine determinds to be the best
 */
Move Engine::getMove(Game g, int timeLeft, int tineIncrement, int timePerMove) {
    int searchTime;
    if (timePerMove == 0) {
        //match time
        searchTime = std::max(timeLeft / 25, tineIncrement + timeLeft / 40);
    } else {
        //Move time
        searchTime = timePerMove;
    }

    std::cout << "searching for " << searchTime << "ms" << std::endl;

    //Starting and stopping the search thread
    std::atomic<bool> stop(false);
    std::thread searchThread(&Search::search, &search, std::ref(g), std::ref(stop));

    std::this_thread::sleep_for(std::chrono::milliseconds(searchTime));
    std::cout << "stopped" << std::endl;
    stop = true;

    if (searchThread.joinable()) {
        searchThread.join(); // Ensures the thread completes execution
    }

    //Picking the best move via whats inside the transposition table
    auto next = g.getAllPseudoLegalMoves();
    return next.moves[search.getNodeFromTable(g)->bestMoveIndex];
}

/*
 * Returns the move the engine determinds to be the best
 */
Move Engine::getMove(Game g, int depth) {

    //Starting and stopping the search thread
    std::atomic<bool> stop(false);
    std::thread searchThread(&Search::searchToDepth, &search, std::ref(g), depth);

    std::cout << "stopped" << std::endl;

    if (searchThread.joinable()) {
        searchThread.join(); // Ensures the thread completes execution
    }

    //Picking the best move via whats inside the transposition table
    auto next = g.getAllPseudoLegalMoves();
    return next.moves[search.getNodeFromTable(g)->bestMoveIndex];
}

//TODO
void Engine::stopSearch() {
    std::cout << "stopped" << std::endl;
}

void Engine::printPrincipalVariation(Game &g) {
    search.printPrincipleVariation(g);
}
