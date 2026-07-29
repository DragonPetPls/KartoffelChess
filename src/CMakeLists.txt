cmake_minimum_required(VERSION 3.27)
project(KartoffelChess)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Apply maximum optimizations for release builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Apply debugging flags for debug builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
endif()

find_package(GTest CONFIG QUIET)
if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(KartoffelChess main.cpp
        Game/Game.h
        Game/constants.h
        Game/Game.cpp
        Game/MagicBitboards.h
        Game/MagicBitboards.cpp
        Game/magicConstants.h
        Game/Move.h
        Engine/Engine.cpp
        Engine/Engine.h
        Engine/Search.h
        Engine/Search.cpp
        Engine/Evaluation.h
        Engine/Evaluation.cpp
        Engine/TranspositionTable.h
        Engine/TranspositionTable.cpp
        Tuner/parameters.h
        Tuner/parameters.cpp
        Communication/Communication.cpp
        Communication/Communication.h
        Communication/Writer.cpp
        Communication/Writer.h
)

add_executable(Tests
        Game/Game.h
        Game/constants.h
        Game/Game.cpp
        Game/MagicBitboards.h
        Game/MagicBitboards.cpp
        Game/magicConstants.h
        Game/Move.h
        Tests/Test.h
        Tests/Test.cpp
        Engine/Engine.cpp
        Engine/Engine.h
        Engine/Search.h
        Engine/Search.cpp
        Engine/Evaluation.h
        Engine/Evaluation.cpp
        Engine/TranspositionTable.h
        Engine/TranspositionTable.cpp
        Tests/GameTest.cpp
        Tests/SearchTest.cpp
        Communication/Writer.cpp
        Communication/Writer.h
)

add_executable(Tuner
        Game/Game.h
        Game/constants.h
        Game/Game.cpp
        Game/MagicBitboards.h
        Game/MagicBitboards.cpp
        Game/magicConstants.h
        Game/Move.h
        Engine/Engine.cpp
        Engine/Engine.h
        Engine/Search.h
        Engine/Search.cpp
        Engine/Evaluation.h
        Engine/Evaluation.cpp
        Engine/TranspositionTable.h
        Engine/TranspositionTable.cpp
        Tuner/parameters.h
        Tuner/parameters.cpp
        Tuner/Tuner.h
        Tuner/Tuner.cpp
        Communication/Writer.cpp
        Communication/Writer.h
)

# linking tests with Game which will be tested
target_link_libraries(Tests
        PRIVATE
        GTest::gtest_main
)