cmake_minimum_required(VERSION 3.3)
project(Evolution)

include_directories(EvolutionLib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")

set(AGGLOMERATIVE_CLUSTERING_SOURCE_FILES
        EvolutionLib/AgglomerativeClustering/AgglomerativeClustering.cpp
        EvolutionLib/AgglomerativeClustering/AgglomerativeClustering.h
        EvolutionLib/AgglomerativeClustering/Cluster.cpp
        EvolutionLib/AgglomerativeClustering/Cluster.hpp
        EvolutionLib/AgglomerativeClustering/DistanceManager.cpp
        EvolutionLib/AgglomerativeClustering/DistanceManager.hpp)

set(DEATH_MANAGER_SOURCE_FILES
        EvolutionLib/DeathManager/DeathManager.c
        EvolutionLib/DeathManager/DeathManager.h)

set(ENTITY_SOURCE_FILES
        EvolutionLib/Entity/Entity.c
        EvolutionLib/Entity/Entity.h)

set(FUNCTIONS_SOURCE_FILES
        EvolutionLib/Functions/TestFunctions.c
        EvolutionLib/Functions/TestFunctions.h)

set(GENETIC_ALGORITHM_SOURCE_FILES
        EvolutionLib/GeneticAlgorithm/GeneticAlgorithm.c
        EvolutionLib/GeneticAlgorithm/GeneticAlgorithm.h)

set(LIST_SOURCE_FILES
        EvolutionLib/List/List.c
        EvolutionLib/List/List.h)

set(LOGGING_SOURCE_FILES
        EvolutionLib/Logging/Logging.c
        EvolutionLib/Logging/Logging.h)

set(SPECIES_SOURCE_FILES
        EvolutionLib/Species/Species.c
        EvolutionLib/Species/Species.h)

set(THREADS_SOURCE_FILES
        EvolutionLib/Threads/Threads.c
        EvolutionLib/Threads/Threads.h)

set(WISHART_SOURCE_FILES
        EvolutionLib/Wishart/Wishart.c
        EvolutionLib/Wishart/Wishart.h)

set(EVOLUTION_LIB_SOURCE_FILES
        ${AGGLOMERATIVE_CLUSTERING_SOURCE_FILES}
        ${DEATH_MANAGER_SOURCE_FILES}
        ${ENTITY_SOURCE_FILES}
        ${FUNCTIONS_SOURCE_FILES}
        ${GENETIC_ALGORITHM_SOURCE_FILES}
        ${LIST_SOURCE_FILES}
        ${LOGGING_SOURCE_FILES}
        ${SPECIES_SOURCE_FILES}
        ${THREADS_SOURCE_FILES}
        ${WISHART_SOURCE_FILES}
        EvolutionLib/Common.c
        EvolutionLib/Common.h
        EvolutionLib/EvolutionLib.c
        EvolutionLib/EvolutionLib.h)

add_library(EvolutionLib
        ${EVOLUTION_LIB_SOURCE_FILES})

add_subdirectory(EvolutionTests)

set(SOURCE_FILES
        main.c)
add_executable(Evolution ${SOURCE_FILES})
target_link_libraries(Evolution EvolutionLib)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(EvolutionLib/Threads REQUIRED)
#target_link_libraries(Evolution Threads::Threads)

target_compile_features(Evolution PRIVATE cxx_range_for)