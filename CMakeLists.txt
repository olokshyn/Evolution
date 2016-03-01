cmake_minimum_required(VERSION 3.3)
project(Wishart)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(LIST_SOURCE_FILES
        List/List.c List/List.h List/ListTests.c List/ListTests.h)
set(WISHART_SOURCE_FILES
        Wishart/Wishart.h Wishart/Wishart.c
        Wishart/WishartTest.c Wishart/WishartTest.h)
set(GENETIC_ALGORITHM_SOURCE_FILES
        GeneticAlgorithm/Simulation.h GeneticAlgorithm/Simulation.c
        GeneticAlgorithm/Lib.h GeneticAlgorithm/Lib.c
        GeneticAlgorithm/GeneticAlgorithm.h GeneticAlgorithm/GeneticAlgorithm.c
        GeneticAlgorithm/TestFunctions.h GeneticAlgorithm/TestFunctions.c)
set(SOURCE_FILES
        main.c
        ${LIST_SOURCE_FILES}
        ${WISHART_SOURCE_FILES}
        ${GENETIC_ALGORITHM_SOURCE_FILES})
add_executable(Wishart ${SOURCE_FILES})