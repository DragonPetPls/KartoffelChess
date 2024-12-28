cmake_minimum_required(VERSION 3.27)
project(KartoffelChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Apply maximum optimizations for release builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Apply debugging flags for debug builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
endif()

# Find libtorch
find_package(Torch REQUIRED)

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
        Search.h
        Search.cpp
        Evaluation.h
        Evaluation.cpp
        Trainer.h
        Trainer.cpp
        TrainingsData.h
        NeuralNet.h
        NeuralNet.cpp
        TrainingsData.cpp
)

# Link libtorch libraries
target_link_libraries(src "${TORCH_LIBRARIES}")

# Ensure RPATH is set correctly to find shared libraries
set_property(TARGET src PROPERTY CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set_property(TARGET src PROPERTY INSTALL_RPATH "$ORIGIN")