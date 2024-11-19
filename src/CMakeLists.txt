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
        Game.h
        constants.h
        Game.cpp
        Test.h
        Test.cpp
        MagicBitboards.h
        MagicBitboards.cpp
        magicConstants.h
        Move.h
        Test.h
        Communication.h
        Communication.cpp
        Engine.cpp
        Engine.h
)
