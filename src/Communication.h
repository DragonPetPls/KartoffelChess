//
// Created by fabian on 10/8/24.
//

#ifndef CHESSENGINE_COMMUNICATION_H
#define CHESSENGINE_COMMUNICATION_H

#define MOVETIME 0
#define MATCHTIME 1
#define USE_DEPTH 2
#define PONDER 3

#define USE_NEURAL_NET

#include <mutex>
#include "Search/Engine.h"
#include <queue>
#include <condition_variable>

#include "Evaluation/Evaluation.h"
#include "Evaluation/NeuralNet.h"
#include "Game/Game.h"

class Communication {
private:
    std::queue<std::string> commandQueue;
    std::mutex queueMtx;
    std::mutex toDo;
    std::mutex output;

    Evaluation evaluator;
    NeuralNet neuralNet;

    std::condition_variable cv;
    Game g;
    Engine e;
    std::mutex gameMtx;

    static char numberToLetter(short x);
    static short letterToNumber(char c);

    void worker();

    void uci();
    void isready();
    void go(const std::string& command);
    void stop();
    void position(std::string command);
public:
    void startCommunication();

#ifndef USE_NEURAL_NET
    Communication() : e(&evaluator) {
    }
#else
    Communication() : e(&neuralNet) {
        bool load = neuralNet.loadFromFile("Weights.dat");
        if (load) {
            std::cout << "Loaded weights from file" << std::endl;
        } else {
            std::cout << "Failed to load weights from file" << std::endl;
        }
    }
#endif

};


#endif //CHESSENGINE_COMMUNICATION_H
