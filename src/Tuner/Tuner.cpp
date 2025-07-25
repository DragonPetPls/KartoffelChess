//
// Created by fabia on 19.07.2025.
//

#include "Tuner.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "../Engine/Engine.h"
#include "../Engine/Evaluation.h"
#include "../Game/Game.h"

void Tuner::playGame(int para1, int para2, int &score1, int &score2) {
    Game g;
    g.loadStartingPosition();

    //Setting up a starting position
    for (int i = 0; i < 6; i++) {
        Moves moves = g.getAllPseudoLegalMoves();
        int r = rand()%moves.moveCount;
        g.doMove(moves.moves[r]);
        if (!g.isPositionLegal()) {
            i--;
            g.undoMove();
            continue;
        }
    }

    Game g2 = g;

    Engine e;
    while (g.getStatus() == ON_GOING) {
        if (g.currentPlayer == WHITE) {
            Parameters::parameter = para1;
        } else {
            Parameters::parameter = para2;
        }
        Move m = e.getMove(g, 0, 0, TIME_PER_MOVE);
        g.doMove(m);
        e.clearTranspositionTable();
    }

    while (g2.getStatus() == ON_GOING) {
        if (g2.currentPlayer == WHITE) {
            Parameters::parameter = para2;
        } else {
            Parameters::parameter = para1;
        }
        Move m = e.getMove(g2, 0, 0, TIME_PER_MOVE);
        g2.doMove(m);
        e.clearTranspositionTable();
    }

    if (g.getStatus() == WHITE_WON) {
        score1 += 2;
    } else if (g.getStatus() == BLACK_WON) {
        score2 += 2;
    } else {
        score1 += 1;
        score2 += 1;
    }
    if (g2.getStatus() == WHITE_WON) {
        score2 += 2;
    } else if (g2.getStatus() == BLACK_WON) {
        score1 += 2;
    } else {
        score1 += 1;
        score2 += 1;
    }
}


bool Tuner::isPara1BetterThanPara2(int para1, int para2) {
    Parameters::parameter = para1;
    double score1 = readData();
    Parameters::parameter = para2;
    double score2 = readData();
    return score1 < score2;
}

void Tuner::tuneParameter() {
    int upperBound = INIT_UPPER_BOUND;
    int lowerBound = INIT_LOWER_BOUND;
    std::cout << lowerBound << " - " << upperBound << std::endl;
    while (lowerBound != upperBound) {
        int low = (2 * lowerBound + upperBound) / 3;
        int high = (lowerBound + 2 * upperBound) / 3;
        if (low == high || low == lowerBound || high == upperBound) {
            if (isPara1BetterThanPara2(lowerBound, upperBound)) {
                upperBound--;
            } else {
                lowerBound++;
            }
        } else {
            if (isPara1BetterThanPara2(low, high)) {
                upperBound = high;
            } else {
                lowerBound = low;
            }
        }
        std::cout << lowerBound << " - " << upperBound << std::endl;
    }
}

void Tuner::generateData() {
    std::ofstream outFile("tuningData.txt", std::ios::app);  // Open file for writing
    if (!outFile.is_open()) {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    Engine e;
    int counter = 0;
    while (true) {
        counter++;
        std::cout << "Game: "<< counter << std::endl;
        Game g;
        g.loadStartingPosition();
        std::vector<Move> moves;
        //Setting up a starting position
        bool restart = false;
        for (int i = 0; i < 6; i++) {
            Moves m = g.getAllPseudoLegalMoves();
            if (m.moveCount == 0) {
                restart = true;
                break;
            }
            int r = rand()%m.moveCount;
            g.doMove(m.moves[r]);
            if (!g.isPositionLegal()) {
                g.undoMove();
                continue;
            }
            moves.push_back(m.moves[r]);
        }
        if (restart == true) {
            counter--;
            continue;
        }

        while (g.getStatus() == ON_GOING) {
            Move m = e.getMove(g, 0, 0, TIME_PER_MOVE);
            e.clearTranspositionTable();
            g.doMove(m);
            moves.push_back(m);
        }

        switch (g.getStatus()) {
            case DRAW:
                std::cout << "D ";
                outFile << "D ";
                break;
            case WHITE_WON:
                std::cout << "W ";
                outFile << "W "; break;
            case BLACK_WON:
                std::cout << "B ";
                outFile << "B ";
             break;
            default: std::cout << "error with status" << std::endl;
        }

        for (int i = 0; i < moves.size(); i++) {
            std::cout << Game::moveToString(moves[i]) << " ";
            outFile << Game::moveToString(moves[i]) << " ";
        }
        outFile << std::endl;
        std::cout << std::endl;
    }
}

double Tuner::readData() {
    std::ifstream inFile("tuningData.txt");  // Open file for reading
    std::string line;
    double mse = 0;
    int positionCounter = 0;
    int counter = 0;
    Search s;

    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            double gameMSE = 0;
            positionCounter = 0;
            counter++;
            std::istringstream iss(line);
            std::vector<std::string> words;
            std::string word;

            while (iss >> word) {
                words.push_back(word);
            }

            Game g;
            g.loadStartingPosition();
            double correctScore = -1;
            if (words[0] == "W") {
                correctScore = 1;
            } else if (words[0] == "B") {
                correctScore = 0;
            } else if (words[0] == "D") {
                correctScore = 0.5;
            } else {
                std::cout << "error with word " << words[0] << std::endl;
            }

            for (int i = 1; i < words.size(); i++) {
                g.doMoveAsString(words[i]);
                positionCounter++;
                int eval = s.quiescence(g, -INF, INF, 0,0);
                    //Evaluation::evaluate(g, -INF);
                if (g.currentPlayer == BLACK) {
                   eval *= -1;
                }
                double dbEval = 1.0 / (1 + exp(-1.0 * eval/SCORE_DIVIDER));
                gameMSE += (correctScore - dbEval) * (correctScore - dbEval);
                positionCounter++;
            }
            s.clearTranspositionTable();
            mse += gameMSE/positionCounter;
        }
        inFile.close();  // Always close the file
    } else {
        std::cerr << "Unable to open file for reading.\n";
    }

    std::cout << std::setprecision(17) << mse/counter << " - " << counter << std::endl;
    return mse;
}
