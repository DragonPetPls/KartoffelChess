//
// Created by fabia on 17.06.2025.
//
#include "../Evaluation/NeuralNet.h"

#ifndef KITRAINER_H
#define KITRAINER_H

class KiTrainer {
private:
    struct Candidate {
        int score = 0;
        NeuralNet nn;
    };

    static void playMatch(Candidate &candidate1, Candidate &candidate2);
public:
    static void train();
};



#endif //KITRAINER_H
