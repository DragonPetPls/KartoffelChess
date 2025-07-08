cmake_minimum_required(VERSION 3.27)
project(KartoffelChess)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Apply maximum optimizations for release builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Apply debugging flags for debug builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
endif()

add_executable(src main.cpp
        Game/Game.h
        Game/constants.h
        Game/Game.cpp
        Game/MagicBitboards.h
        Game/MagicBitboards.cpp
        Game/magicConstants.h
        Game/Move.h
        Tests/Test.h
        Tests/Test.cpp
        Communication.h
        Communication.cpp
        Search/Engine.cpp
        Search/Engine.h
        Search/Search.h
        Search/Search.cpp
        Search/TranspositionTable.h
        Search/TranspositionTable.cpp
        Evaluation/Evaluation.h
        Evaluation/Evaluation.cpp
        Search/MoveOrderer.cpp
        Search/MoveOrderer.h
        Evaluation/StaticEvaluation.h
        KITest/KiTrainer.cpp
        KITest/KiTrainer.h
        Evaluation/NeuralNet.cpp
        Evaluation/NeuralNet.h
)
