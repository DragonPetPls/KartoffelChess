//
// Created by fabia on 17.06.2025.
//

#ifndef NEURALNET_H
#define NEURALNET_H
#include "StaticEvaluation.h"

class NeuralNet : public StaticEvaluation{

    constexpr static int PIECE_TYPES = 6;

private:
    void convertGameToArray(const Game &g, int array[8][8][PIECE_TYPES]);
    int evaluateNetwork(const Game &g);

public:
    int evaluate(const Game &g) override;

};



#endif //NEURALNET_H
