//
// Created by fabia on 17.06.2025.
//

#ifndef NEURALNET_H
#define NEURALNET_H
#include <string>

#include "StaticEvaluation.h"

constexpr int FILTER_COUNT = 10;
constexpr int FILTER_SIZE = 3;
constexpr float MUTATION_RATE = 10; //In Percent

class NeuralNet : public StaticEvaluation{
    constexpr static int PIECE_TYPES = 6;

private:
    float filters[FILTER_COUNT][FILTER_SIZE][FILTER_SIZE][PIECE_TYPES] = {};
    float filterBiases[FILTER_COUNT][8][8] = {};
    float linear[FILTER_COUNT][8][8] = {};

    void convertGameToArray(const Game &g, int array[10][10][PIECE_TYPES]);
    int evaluateNetwork(const Game &g);

public:
    void randomInitialization();
    void mutateValues(float addMax, float multiMax);
    int evaluate(const Game &g) override;

    bool saveToFile(const std::string &filename) const;
    bool loadFromFile(const std::string &filename);

};



#endif //NEURALNET_H
