//
// Created by fabian on 12/28/24.
//

#include "Trainer.h"

void Trainer::train() {
    while (true) {
        Game g = TrainingsData::getRandomPosition();
        g.printGame();
    }
}
