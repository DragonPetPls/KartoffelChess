//
// Created by fabia on 19.07.2025.
//

#ifndef TUNER_H
#define TUNER_H
#include "parameters.h"

class Tuner {
private:
    static constexpr int INIT_LOWER_BOUND = -100;
    static constexpr int INIT_UPPER_BOUND = 500;
    static constexpr int MAX_GAMES_PER_COLOR = 100;
    static constexpr int REQUIRED_LEAD = 10;
    static constexpr int TIME_PER_MOVE = 100;

    static int playGame(int para1, int para2);
    static bool isPara1BetterThanPara2(int para1, int para2);
public:
    static void tuneParameter();
};



#endif //TUNER_H
