cmake_minimum_required(VERSION 3.3)
project(Evolution)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -Wno-unused-function -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-unused-function -Werror -std=c++11")

add_subdirectory(EvolutionLib)
add_subdirectory(EvolutionTests)
add_subdirectory(Plugins)
add_subdirectory(Console)
add_subdirectory(GUI)

add_dependencies(Console EvolutionTests ${PLUGINS})
add_dependencies(GUI EvolutionTests ${PLUGINS})
