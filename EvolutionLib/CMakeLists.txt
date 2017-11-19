cmake_minimum_required(VERSION 3.3)
project(EvolutionLib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-function -Wno-format-security -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Werror -std=c++11")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(AGGLOMERATIVE_CLUSTERING_SOURCE_FILES
        AgglomerativeClustering/AgglomerativeClustering.cpp
        AgglomerativeClustering/AgglomerativeClustering.h
        AgglomerativeClustering/Cluster.cpp
        AgglomerativeClustering/Cluster.hpp
        AgglomerativeClustering/DistanceManager.cpp
        AgglomerativeClustering/DistanceManager.hpp)

set(ENTITY_SOURCE_FILES
        Entity/Entity.c
        Entity/Entity.h
        Entity/CrossoverLib.h
        Entity/Crossovers.c
        Entity/Crossovers.h)

set(FUNCTIONS_SOURCE_FILES
        Functions/TestFunctions.c
        Functions/TestFunctions.h)

set(GENETIC_ALGORITHM_SOURCE_FILES
        GeneticAlgorithm/GeneticAlgorithm.c
        GeneticAlgorithm/GeneticAlgorithm.h
        GeneticAlgorithm/GALib.c
        GeneticAlgorithm/GALib.h
        GeneticAlgorithm/GAFwd.h
        GeneticAlgorithm/World.c
        GeneticAlgorithm/World.h
        GeneticAlgorithm/GAParameters.h
        GeneticAlgorithm/GAOperators.h
        GeneticAlgorithm/GAOperators.c)

set(LIST_SOURCE_FILES
        List/List.c
        List/List.h)

set(LOGGING_SOURCE_FILES
        Logging/Logging.c
        Logging/Logging.h)

set(JOURNAL_SOURCE_FILES
        Journal/Journal.c
        Journal/Journal.h)

set(SPECIES_SOURCE_FILES
        Species/Species.c
        Species/Species.h)

#set(THREADS_SOURCE_FILES
#        Threads/Threads.c
#        Threads/Threads.h)

set(WISHART_SOURCE_FILES
        Wishart/Wishart.c
        Wishart/Wishart.h)

set(OPTICS_SOURCE_FILES
        OPTICS/optics.cpp
        OPTICS/optics.h
        OPTICS/PrioritizedQueue.h
        OPTICS/OpticsObject.h
        OPTICS/OrderedSeeds.cpp
        OPTICS/OrderedSeeds.h
        OPTICS/OpticsObjectsManager.cpp
        OPTICS/OpticsObjectsManager.h)

set(EVOLUTION_LIB_SOURCE_FILES
        ${ENTITY_SOURCE_FILES}
        ${FUNCTIONS_SOURCE_FILES}
        ${GENETIC_ALGORITHM_SOURCE_FILES}
        ${LIST_SOURCE_FILES}
        ${LOGGING_SOURCE_FILES}
        ${JOURNAL_SOURCE_FILES}
        ${SPECIES_SOURCE_FILES}
        ${WISHART_SOURCE_FILES}
        ${AGGLOMERATIVE_CLUSTERING_SOURCE_FILES}
        ${OPTICS_SOURCE_FILES}
        Common.c
        Common.h)

add_library(EvolutionLib ${EVOLUTION_LIB_SOURCE_FILES})
