cmake_minimum_required(VERSION 3.26)

project(sim
    LANGUAGES CXX C
    DESCRIPTION "Small & insignificant simulator")

add_subdirectory(3rd-party)
find_package(Vulkan REQUIRED)

add_executable(sim
        src/main.cpp
        src/vulkan.cpp
        include/sim/engine.hpp
        src/sim.cpp)
target_include_directories(sim
        PUBLIC include/)
target_compile_features(sim
        PRIVATE cxx_std_23)
target_link_libraries(sim
        PRIVATE glfw glm::glm Vulkan::Vulkan)