cmake_minimum_required(VERSION 3.26)

project(sim
    LANGUAGES CXX C
    DESCRIPTION "Small & insignificant simulator")

add_subdirectory(3rd-party)
find_package(Vulkan REQUIRED)

add_executable(sim
        src/main.cpp
        src/vulkan.cpp
        src/sim.cpp)
target_include_directories(sim
        PUBLIC include/)
target_compile_features(sim
        PRIVATE cxx_std_23)

if($CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(sim
            PRIVATE DEBUG_BUILD)
endif()

target_link_libraries(sim
        PRIVATE glfw glm::glm Vulkan::Vulkan)