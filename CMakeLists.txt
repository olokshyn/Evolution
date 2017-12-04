cmake_minimum_required(VERSION 3.3)
project(Evolution)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-function -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Werror -std=c++11")

add_subdirectory(EvolutionLib)
add_subdirectory(EvolutionTests)
add_subdirectory(Console)
add_subdirectory(GUI)

add_dependencies(Console EvolutionTests)
add_dependencies(GUI EvolutionTests)
