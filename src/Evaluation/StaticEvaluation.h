//
// Created by fabia on 17.06.2025.
//

#ifndef STATICEVALUATION_H
#define STATICEVALUATION_H

class Game;

class StaticEvaluation {
public:
    virtual int evaluate(const Game &g) = 0;
};


#endif //STATICEVALUATION_H
