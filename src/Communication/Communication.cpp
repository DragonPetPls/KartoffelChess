//
// Created by fabian on 10/8/24.
//

#include <queue>
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include "Communication.h"

#include "Writer.h"
#include "../Engine/Evaluation.h"
#include "../Game/Game.h"

void Communication::startCommunication() {
    g.loadStartingPosition();
    std::thread worker(&Communication::worker, this);

    //Adds all inputs at the end of the command queue
    std::string lastInput;
    while(lastInput != "quit"){
        getline(std::cin, lastInput);
        queueMtx.lock();
        commandQueue.push(lastInput);
        queueMtx.unlock();
        cv.notify_one();
    }

    if(worker.joinable()) {
        worker.join();
    }
}



void Communication::uci() {
    output.lock();
    Writer::print("id name KartoffelChess", "uci");
    Writer::print("id author Fabian", "uci");
    Writer::print("v2", "uci");
    Writer::print("uciok", "uci");
    output.unlock();
}

void Communication::isready() {
    output.lock();
    Writer::print("readyok", "uci");
    output.unlock();
}

void Communication::go(const std::string& command) {
    int moveTime = 5000;
    int increment = 0;
    int timeLeft = 0;
    int timeMode = MOVETIME;
    int depth = 0;

    bool isTimeSet = false;

    std::istringstream stream(command);
    std::string argument;
    std::vector<std::string> arguments;

    while (stream >> argument) {
        arguments.push_back(argument);
    }

    //Checking what time is relevant for the engine:
    std::string timeStr;
    std::string incStr;
    if(g.currentPlayer == WHITE){
        timeStr = "wtime";
        incStr = "winc";
    } else {
        timeStr = "btime";
        incStr = "binc";
    }

    for(int i = 0; i < arguments.size(); i++){
        if(arguments[i] == "movetime"){
            std::stringstream ss(arguments[i + 1]);
            ss >> moveTime;
            timeMode = MOVETIME;
        }
        if(arguments[i] == timeStr){
            std::stringstream ss(arguments[i + 1]);
            ss >> timeLeft;
            timeMode = MATCHTIME;
        }
        if(arguments[i] == incStr){
            std::stringstream ss(arguments[i + 1]);
            ss >> increment;
            timeMode = MATCHTIME;
        }
        if(arguments[i] == "depth") {
            std::istringstream(arguments[i + 1]) >> depth;
            timeMode = USE_DEPTH;
        }
        if(arguments[i] == "ponder") {
            timeMode = PONDER;
        }
    }

    if(timeMode == MATCHTIME){
        moveTime = 0;
    }

    gameMtx.lock();
    Move m;
    if (timeMode == PONDER) {
        e.ponder(g);
        gameMtx.unlock();
        return;
    } else if(timeMode != USE_DEPTH) {
        m = e.getMove(g, timeLeft, increment, moveTime);
    } else {
        m = e.getMove(g, depth);
    }

    gameMtx.unlock();
    output.lock();

    std::string out = "bestmove " + Game::moveToString(m);
    if(m.startingPiece != m.endingPiece) {
        switch (m.endingPiece) {
            case QUEEN: out += "q"; break;
            case ROOK: out += "r"; break;
            case BISHOP: out += "b"; break;
            case KNIGHT: out += "n"; break;
            default: break;
        }
    }
    Writer::print(out, "uci");
    output.unlock();
}

void Communication::stop() {
    e.stopSearch();
}

char Communication::numberToLetter(short x) {

    switch(x){
        case 0: return 'a';
        case 1: return 'b';
        case 2: return 'c';
        case 3: return 'd';
        case 4: return 'e';
        case 5: return 'f';
        case 6: return 'g';
        case 7: return 'h';
        default: return ' ';
    }

    return 0;
}

void Communication::position(std::string command) {
    e.clearTranspositionTable();
    std::istringstream stream(command);
    std::string argument;
    std::vector<std::string> arguments;

    int moveIndex = 2;

    while (stream >> argument) {
        arguments.push_back(argument);
    }

    if(arguments.size() < 2){
        return;
    }

    if(arguments[1] == "startpos"){
        g.loadStartingPosition();

    } else if (arguments[1] == "fen"){
        std::string fen;
        for (int i = 2; i < arguments.size(); i++){
            if(arguments[i] == "moves"){
                moveIndex = i;
                break;
            }
            fen += arguments[i] + " ";
        }
        g.loadFen(fen);
    }

    if (arguments.size() < moveIndex + 1){
        return;
    }

    if(arguments[moveIndex] == "moves"){
        for(int i = moveIndex + 1; i < arguments.size(); i++){
            g.doMoveAsString(arguments[i]);
        }
    }

}

short Communication::letterToNumber(char c) {

    switch(c){
        case 'a': return 0;
        case 'b': return 1;
        case 'c': return 2;
        case 'd': return 3;
        case 'e': return 4;
        case 'f': return 5;
        case 'g': return 6;
        case 'h': return 7;
        default: return 9;
    }


    return 0;
}

void Communication::worker() {
    std::string command;
    std::string subcommand;

    while(command != "quit"){
        queueMtx.lock();
        if(commandQueue.empty()){
            queueMtx.unlock();
            std::unique_lock<std::mutex> lg(toDo);
            cv.wait(lg, [this] {return (!commandQueue.empty());});
            continue;
        }

        command = commandQueue.front();
        commandQueue.pop();
        queueMtx.unlock();

        subcommand = command.substr(0, command.find(" "));

        if(command == "uci"){
            uci();
        } else if (command == "isready"){
            isready();
        } else if (subcommand == "position"){
            position(command);
        } else if (subcommand == "go"){
            std::thread(&Communication::go, this, command).detach();
        } else if (subcommand == "eval"){
            Writer::print("Eval: " + std::to_string(Evaluation::evaluate(g, -INF)), "uci");
        } else if (subcommand == "principal") {
            e.printPrincipalVariation(g);
        } else  if (subcommand == "print") {
            g.printGame();
        } else if (subcommand == "stop") {
            e.stopSearch();
        }
    }
}
