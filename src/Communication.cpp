//
// Created by fabian on 10/8/24.
//

#include <queue>
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include "Communication.h"

#include "Evaluation.h"
#include "Game.h"

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
    std::cout << "id name KartoffelBot" << std::endl;
    std::cout << "id author Fabian" << std::endl;
    std::cout << "v2" << std::endl;
    std::cout <<  "uciok" << std::endl;
    output.unlock();
}

void Communication::isready() {
    output.lock();
    std::cout << "readyok" << std::endl;
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

    for(int i = 0; i < arguments.size() - 1; i++){
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
    }

    if(timeMode == MATCHTIME){
        moveTime = 0;
    }

    gameMtx.lock();
    Move m;
    if(timeMode != USE_DEPTH) {
        m = e.getMove(g, timeLeft, increment, moveTime);
    } else {
        m = e.getMove(g, depth);
    }

    gameMtx.unlock();
    output.lock();

    int fromIndex = Game::getIndex(m.fromSquare);
    int fromX = fromIndex % 8;
    int fromY = fromIndex / 8;
    int toIndex = Game::getIndex(m.toSquare);
    int toX = toIndex % 8;
    int toY = toIndex / 8;
    std::cout << "bestmove " << numberToLetter(fromX) << (fromY + 1)
    << numberToLetter(toX) << toY + 1;
    if(m.startingPiece != m.endingPiece) {
        switch (m.endingPiece) {
            case QUEEN: std::cout << "q"; break;
            case ROOK: std::cout << "r"; break;
            case BISHOP: std::cout << "b"; break;
            case KNIGHT: std::cout << "n"; break;
            default: std::cout << std::endl;
        }
    }
    std::cout << std::endl;
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
#ifdef DEBUG
            output.lock();
            std::cout << m.getFromHorizontal() << m.getFromVertical() << m.getToHorizontal() << m.getToVertical() << m.getPromoteTo() << std::endl;
            g.printGame();
            output.unlock();
#endif
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
            std::cout << "Eval: " << Evaluation::evaluate(g) << std::endl;
        } else if (subcommand == "principal") {
            e.printPrincipalVariation(g);
        } else  if (subcommand == "print") {
            g.printGame();
        }
    }
}
