//
// Created by fabia on 19.07.2025.
//

#ifndef TUNER_H
#define TUNER_H
#include "parameters.h"

class Tuner {
private:
    static constexpr int INIT_LOWER_BOUND = 1;
    static constexpr int INIT_UPPER_BOUND = 10000;
    static constexpr int MAX_GAMES_PER_COLOR = 1000000;
    static constexpr int REQUIRED_LEAD = 50;

    static constexpr int TIME_PER_MOVE = 50;
    static constexpr int SCORE_DIVIDER = 164;

    static void playGame(int para1, int para2, int &score1, int &score2);
    static bool isPara1BetterThanPara2(int para1, int para2);
public:
    static void tuneParameter();
    static void generateData();
    static double readData();
};



#endif //TUNER_H
