//
// Created by fabia on 17.06.2025.
//

#include "NeuralNet.h"

#include <fstream>

#include "../Game/Game.h"

/*
 * Fills the array with the pieces.
 * 0,0 is the bottom left corner. Board is flipped when black is to move,
 * assumes the array is empty (0 everywhere) when passed
 */
void NeuralNet::convertGameToArray(const Game &g, int array[10][10][PIECE_TYPES]) {
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
                    array[x + 1][y + 1][pieceType] = 1;
                } else if (g.pieceBoards[pieceType | enemyColor] & (one << index)) {
                    array[x + 1][y + 1][pieceType] = -1;
                }
            }
        }
    }
}

/*
 * Evaluates the network
 */
int NeuralNet::evaluateNetwork(const Game &g) {
    int input[10][10][PIECE_TYPES] = {};
    NeuralNet::convertGameToArray(g, input);

    float hidden[FILTER_COUNT][8][8];

    //Appling filters
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            for (int f = 0; f < FILTER_COUNT; f++) {

                float sum = 0;
                for (int fx = 0; fx < FILTER_SIZE; fx++) {
                    for (int fy = 0; fy < FILTER_SIZE; fy++) {
                        for (int p = 0; p < PIECE_TYPES; p++) {
                            sum += input[x + fx][y + fx][p] * filters[f][fx][fy][p];
                        }
                    }
                }
                hidden[f][x][y] = std::max(0.0f, sum + filterBiases[f][x][y]); //Relu
            }
        }
    }

    //Output Layer
    float output = 0.0f;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            for (int f = 0; f < FILTER_COUNT; f++) {
               output += linear[f][x][y] * hidden[f][x][y];
            }
        }
    }

    return output * 100;
}

/*
 * Return the evaluationn from the network
 */
int NeuralNet::evaluate(const Game &g) {
    return evaluateNetwork(g);
}

bool NeuralNet::saveToFile(const std::string &filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(filters), sizeof(filters));
    out.write(reinterpret_cast<const char*>(filterBiases), sizeof(filterBiases));
    out.write(reinterpret_cast<const char*>(linear), sizeof(linear));
    return out.good();
}

bool NeuralNet::loadFromFile(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;
    in.read(reinterpret_cast<char*>(filters), sizeof(filters));
    in.read(reinterpret_cast<char*>(filterBiases), sizeof(filterBiases));
    in.read(reinterpret_cast<char*>(linear), sizeof(linear));

    if (in.good()) {
        std::cout << "Loaded file " << filename << std::endl;
    } else {
        std::cout << "Failed to load file " << filename << std::endl;
    }

    return in.good();
}

/*
 * Sets all weights to a random value
 */
void NeuralNet::randomInitialization() {
    for (int i = 0; i < FILTER_COUNT; i++) {
        for (int x = 0; x < FILTER_SIZE; x++) {
            for (int y = 0; y < FILTER_SIZE; y++) {
                for (int z = 0; z < FILTER_COUNT; z++) {
                    filters[i][x][y][z] = ((rand() % 1000) - 500)/100.0;
                }
            }
        }
    }

    for (int i = 0; i < FILTER_COUNT; i++) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
               linear[i][x][y] = ((rand() % 1000) - 500)/100.0;
                filterBiases[i][x][y] = ((rand() % 1000) - 500)/100.0;
            }
        }
    }
}

/*
 * Causes slight changes in the weights
 */
void NeuralNet::mutateValues(float addMax, float multiMax) {
    for (int i = 0; i < FILTER_COUNT; i++) {
        for (int x = 0; x < FILTER_SIZE; x++) {
            for (int y = 0; y < FILTER_SIZE; y++) {
                for (int z = 0; z < FILTER_COUNT; z++) {
                    int r = rand() % 100;
                    if (r < MUTATION_RATE) {
                        float addChange = ((rand() % 200)/100.0f - 1)  * addMax;
                        float multiChange = 1 + ((rand() % 200)/100.0f - 1) * multiMax;
                        filters[i][x][y][z] = filters[i][x][y][z] * multiChange + addChange;
                    }
                }
            }
        }
    }

    for (int i = 0; i < FILTER_COUNT; i++) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                int r = rand() % 100;
                if (r < MUTATION_RATE) {
                    float addChange = ((rand() % 200)/100.0f - 1)  * addMax;
                    float multiChange = 1 + ((rand() % 200)/100.0f - 1) * multiMax;
                    linear[i][x][y] = linear[i][x][y] * multiChange + addChange;
                }
                r = rand() % 100;
                if (r < MUTATION_RATE) {
                    float addChange = ((rand() % 200)/100.0f - 1)  * addMax;
                    float multiChange = 1 + ((rand() % 200)/100.0f - 1) * multiMax;
                    filterBiases[i][x][y] = filterBiases[i][x][y] * multiChange + addChange;
                }
            }
        }
    }
}