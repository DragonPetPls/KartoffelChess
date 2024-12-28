//
// Created by fabian on 12/28/24.
//

#ifndef TRAININGSDATA_H
#define TRAININGSDATA_H
#include "Engine.h"
#include "Game.h"
#include <torch/data/datasets/base.h>

constexpr long INPUT_SIZE = 384; //64 squares * 6 piece types
constexpr long OUTPUT_SIZE = 1;

constexpr int RANDOM_MOVE_CHANCE_PERCENTAGE = 33;
class TrainingsData {
public:
    static Game getRandomPosition();
};

struct DataSet : torch::data::datasets::Dataset<DataSet>{

    torch::data::Example<> get(size_t index) override {
        auto data = torch::randn({INPUT_SIZE});
        auto target = torch::randint(0, 2, {OUTPUT_SIZE});
        return {data, target};
    }

    // Define the size of the dataset
    torch::optional<size_t> size() const override {
        return 10000000; // A large number cause we have essentially infinite data point cause we generate them on the fly
    }
};

#endif //TRAININGSDATA_H
