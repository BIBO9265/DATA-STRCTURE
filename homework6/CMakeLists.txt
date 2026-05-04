cmake_minimum_required(VERSION 3.16)

project(homework2_graph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(homework2_graph
    src/main.cpp
    src/graph.cpp
    src/maze.cpp
    src/algorithms.cpp
)

target_include_directories(homework2_graph PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
