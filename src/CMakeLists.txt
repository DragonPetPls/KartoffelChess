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
        Engine/Engine.cpp
        Engine/Engine.h
        Engine/Search.h
        Engine/Search.cpp
        Engine/Evaluation.h
        Engine/Evaluation.cpp
        Engine/TranspositionTable.h
        Engine/TranspositionTable.cpp
)
