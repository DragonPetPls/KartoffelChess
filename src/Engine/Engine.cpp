//
// Created by fabian on 11/19/24.
//

#include "Engine.h"

#include <chrono>
#include <thread>

#include "Search.h"
#include "../Communication/Writer.h"

/*
 * Returns the move the engine determinds to be the best
 */
Move Engine::getMove(Game g, int timeLeft, int tineIncrement, int timePerMove) {

    stopSearch();

    int searchTime;
    if (timePerMove == 0) {
        //match time
        searchTime = std::max(timeLeft / 25, tineIncrement + timeLeft / 40);
    } else {
        //Move time
        searchTime = timePerMove;
    }

    Writer::print("searching for " + std::to_string(searchTime) + "ms", "info");

    //Starting and stopping the search thread
    std::atomic<bool> stop(false);
    std::thread searchThread(&Search::search, &search, std::ref(g), std::ref(stop));

    std::this_thread::sleep_for(std::chrono::milliseconds(searchTime));

    Writer::print("stopped", "info");

    stop = true;

    if (searchThread.joinable()) {
        searchThread.join(); // Ensures the thread completes execution
    }

    //Picking the best move via whats inside the transposition table
    auto next = g.getAllPseudoLegalMoves();
    Move bestMove = next.moves[search.getNodeFromTable(g).bestMoveIndex];
    return bestMove;
}

/*
 * Returns the move the engine determinds to be the best
 */
Move Engine::getMove(Game g, int depth) {

    stopSearch();

    //Starting and stopping the search thread
    std::thread searchThread(&Search::searchToDepth, &search, std::ref(g), depth);

    Writer::print("stopped", "info");

    if (searchThread.joinable()) {
        searchThread.join(); // Ensures the thread completes execution
    }

    //Picking the best move via whats inside the transposition table
    auto next = g.getAllPseudoLegalMoves();
    Node n = search.getNodeFromTable(g);
    Move bestMove = next.moves[search.getNodeFromTable(g).bestMoveIndex];
    return bestMove;
}

void Engine::ponder(Game g) {
    stop = false;
    thread = std::make_unique<std::thread>(std::thread(&Search::ponder, &search, g, std::ref(stop)));
}

void Engine::stopSearch() {
    if(this->thread != nullptr) {
        stop = true;
        Writer::print("stopping ponder thread", "info");
        if(thread->joinable()) {
            thread->join();
        }
    }
}

void Engine::printPrincipalVariation(Game &g) {
    search.printPrincipleVariation(g);
}
