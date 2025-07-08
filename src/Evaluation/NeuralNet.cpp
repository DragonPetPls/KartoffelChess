//
// Created by fabia on 17.06.2025.
//

#include "NeuralNet.h"
#include "../Game/Game.h"

/*
 * Fills the array with the pieces.
 * 0,0 is the bottom left corner. Board is flipped when black is to move,
 * assumes the array is empty (0 everywhere) when passed
 */
void NeuralNet::convertGameToArray(const Game &g, int array[8][8][PIECE_TYPES]) {
    bool flipBoard = g.currentPlayer == BLACK;
    uint8_t color = COLOR_TO_PIECE[g.currentPlayer];
    uint8_t enemyColor = COLOR_TO_PIECE[1 - g.currentPlayer];

    for (int pieceType = 0; pieceType < PIECE_TYPES; pieceType++) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                int yCord = flipBoard ? 7 - y : y;
                int index = yCord * 8 + x;
                uint64_t one = 1;
                if (g.pieceBoards[pieceType | color] & (one << index)) {
                    array[x][y][pieceType] = 1;
                } else if (g.pieceBoards[pieceType | enemyColor] & (one << index)) {
                    array[x][y][pieceType] = -1;
                } else {
                    array[x][y][pieceType] = 0;
                }
            }
        }
    }
}

/*
 * Evaluates the network
 */
int NeuralNet::evaluateNetwork(const Game &g) {
    int a[8][8][PIECE_TYPES];
    NeuralNet::convertGameToArray(g, a);
    return 1;
}

/*
 * Return the evaluationn from the network
 */
int NeuralNet::evaluate(const Game &g) {
    return evaluateNetwork(g);
}
