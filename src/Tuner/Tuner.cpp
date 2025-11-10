//
// Created by fabia on 19.07.2025.
//

#include "Tuner.h"
#include "../Engine/Engine.h"
#include "../Game/Game.h"

int Tuner::playGame(int para1, int para2) {
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
        Move m = e.getMove(g2, 0, 0, 5);
        g2.doMove(m);
        e.clearTranspositionTable();
    }

    if (g.getStatus() == WHITE_WON && g2.getStatus() == BLACK_WON) {
        return 1;
    } else if (g.getStatus() == BLACK_WON && g2.getStatus() == WHITE_WON) {
        return 2;
    } else {
        return 0;
    }
}


bool Tuner::isPara1BetterThanPara2(int para1, int para2) {
    int score1 = 0;
    int score2 = 0;
    for (int i = 0; i < MAX_GAMES_PER_COLOR; i++) {
        int result = playGame(para1, para2);
        if (result == 1) {
            score1++;
        } else if (result == 2) {
            score2++;
        }
        if (abs(score1 - score2) >= REQUIRED_LEAD) {
            break;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (score1 != score2) {
            continue;
        }
        int result = playGame(para1, para2);
        if (result == 1) {
            score1++;
        } else if (result == 2) {
            score2++;
        }
    }

    return score1 > score2;
}

void Tuner::tuneParameter() {
    int upperBound = INIT_UPPER_BOUND;
    int lowerBound = INIT_LOWER_BOUND;
    while (true) {
        std::cout << lowerBound << " - " << upperBound << std::endl;
        int low = (2 * lowerBound + upperBound) / 3;
        int high = (lowerBound + 2 * upperBound) / 3;
        if (isPara1BetterThanPara2(low, high)) {
            upperBound = high;
        } else {
            lowerBound = low;
        }
    }
}

int main() {
    Tuner::tuneParameter();
}