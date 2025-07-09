//
// Created by fabia on 17.06.2025.
//

#include "KiTrainer.h"
#include "../Game/Game.h"
#include "../Search/Engine.h"

#include <iostream>
#include <ostream>
#include <fstream>

#include "../Evaluation/Evaluation.h"

constexpr int POPULATION_COUNT = 10;
constexpr int MIN_GAMES = 3;

void KiTrainer::playMatch(Candidate &candidate1, Candidate &candidate2) {
    Game g1;
    g1.loadStartingPosition();
    for (int i = 0; i < 4; i++) {
        auto moves = g1.getAllPseudoLegalMoves();
        int r = (rand() % moves.moveCount);
        g1.doMove(moves.moves[r]);
        if (!g1.isPositionLegal()) {
            g1.undoMove();
        }
    }
    Game g2 = g1;
    color startingColor = g1.currentPlayer;

    Engine e1(&candidate1.nn);
    Engine e2(&candidate2.nn);
    Engine* e[2] = {&e1, &e2};

    int currentPlayer = 0;
    //g1.printGame();
    while (g1.getStatus() == ON_GOING) {
        g1.doMove(e[currentPlayer]->getMove(g1, 1));
        currentPlayer = 1 - currentPlayer;
        if (g1.getGameHistoryCounter() > 30) {
            break;
        }
    }
    //g1.printGame();

    currentPlayer = 1;
    while (g2.getStatus() == ON_GOING) {
        g2.doMove(e[currentPlayer]->getMove(g2, 1));
        currentPlayer = 1 - currentPlayer;
        if (g2.getGameHistoryCounter() > 30) {
            break;
        }
    }
    //g2.printGame();
    if (g1.getStatus() == WHITE_WON && startingColor == WHITE || g1.getStatus() == BLACK_WON && startingColor == BLACK) {
        candidate1.score++;
        candidate2.score--;
    } else if (g1.getStatus() == BLACK_WON && startingColor == WHITE || g1.getStatus() == WHITE_WON && startingColor == BLACK) {
        candidate1.score--;
        candidate2.score++;
    } else if (g1.getStatus() == ON_GOING) {
        Evaluation eval;
        if (eval.evaluate(g1) > 0 && startingColor == g1.currentPlayer || eval.evaluate(g1) < 0 && startingColor != g1.currentPlayer) {
            candidate1.score++;
            candidate2.score--;
        } else {
            candidate1.score--;
            candidate2.score++;
        }
    }

    if (g2.getStatus() == WHITE_WON && startingColor == WHITE || g2.getStatus() == BLACK_WON && startingColor == BLACK) {
        candidate2.score++;
        candidate1.score--;
    } else if (g2.getStatus() == BLACK_WON && startingColor == WHITE || g2.getStatus() == WHITE_WON && startingColor == BLACK) {
        candidate2.score--;
        candidate1.score++;
    } else if (g2.getStatus() == ON_GOING) {
        Evaluation eval;
        if (eval.evaluate(g2) > 0 && startingColor == g2.currentPlayer || eval.evaluate(g2) < 0 && startingColor != g2.currentPlayer) {
            candidate2.score++;
            candidate1.score--;
        } else {
            candidate2.score--;
            candidate1.score++;
        }
    }
}

void KiTrainer::train() {
    std::cout << "Training..." << std::endl;
    std::vector<Candidate> candidates;
    for (int i = 0; i < POPULATION_COUNT; i++) {
        candidates.emplace_back();
        candidates[i].nn.randomInitialization();
    }

    //Trying to load from file
    bool load = candidates[0].nn.loadFromFile("Weights.dat");
    if (load) {
        std::cout << "Loaded weights" << std::endl;
    } else {
        std::cout << "No weights loaded." << std::endl;
    }

    int generation = 0;
    while (generation < 0) {
        for (int i = 0; i < POPULATION_COUNT; i++) {
            for (int j = 0; j < MIN_GAMES
                ; j++) {
                int r = rand() % POPULATION_COUNT;
                if (r == i) {
                    j--;
                    continue;
                }
                playMatch(candidates[i], candidates[j]);
            }
        }

        std::vector<int> winners;
        std::vector<int> losers;
        for (int i = 0; i < POPULATION_COUNT; i++) {
            if (candidates[i].score > 0) {
                winners.emplace_back(i);
            } else if (candidates[i].score < 0) {
                losers.emplace_back(i);
            } else {
                int r = rand()%2;
                if (r == 0 || winners.size() == 0) {
                    winners.emplace_back(i);
                } else if (r == 1) {
                    losers.emplace_back(i);
                }
            }
        }

        for (auto i : losers) {
            int r = rand()%(winners.size() + 1);
            if (r == winners.size()) {
                candidates[i].nn.randomInitialization();
            } else {
                candidates[i].nn = candidates[winners[r]].nn;
                candidates[i].nn.mutateValues(1, 0.2);
            }
        }

        for (int i = 0; i < POPULATION_COUNT; i++) {
            candidates[i].score = 0;
        }

        std::cout << "finished " << generation++ << std::endl;

        // === Write to a file ===
        bool save = candidates[winners[0]].nn.saveToFile("Weights.dat");
        if (save) {
            std::cout << "saved to file" << std::endl;
        } else {
            std::cout << "saved to file failed" << std::endl;
        }
    }

    std::cout << "Test Matches:" << std::endl;
    candidates[0].score = 0;
    Candidate opponent;
    opponent.score = 0;
    for (int i = 0; i < 1000; i++) {
        opponent.nn.randomInitialization();
        playMatch(candidates[0], opponent);
        std::cout << "Score to Matches: " << candidates[0].score * 1.0 / i << " i: " << i << std::endl;
    }
}
