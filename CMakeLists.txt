cmake_minimum_required(VERSION 3.24)
project(heuristic_makespan)

set(CMAKE_CXX_STANDARD 17)

add_executable(heuristic_makespan main.cpp busca_tabu.cpp)
