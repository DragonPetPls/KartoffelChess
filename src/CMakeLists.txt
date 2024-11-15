cmake_minimum_required(VERSION 3.27)
project(KartoffelChess)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2")


add_executable(src main.cpp
        Game.h
        constants.h
        Game.cpp
        Test.h
        Test.cpp
        MagicBitboards.h
        MagicBitboards.cpp
        magicConstants.h
)
