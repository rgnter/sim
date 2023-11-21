cmake_minimum_required(VERSION 3.26)

project(sim
    LANGUAGES CXX
    DESCRIPTION "Small & insiginificant simulator")

add_executable(sim
        src/main.cpp
        src/math.cpp
        src/sim.cpp)
target_include_directories(sim
        PUBLIC include/)

target_compile_features(sim
        PRIVATE cxx_std_20)