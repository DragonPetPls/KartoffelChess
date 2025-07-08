//
// Created by fabia on 17.06.2025.
//

#include "KiTrainer.h"
#include "../Game/Game.h"

#include <iostream>
#include <ostream>

void KiTrainer::train() {
    Game g;
    g.loadStartingPosition();
    NeuralNet nn;
    nn.evaluate(g);
}
