project(EvolutionLib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-function -Wno-format-security -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unused-function -Werror")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB AGGLOMERATIVE_CLUSTERING_SOURCE_FILES
        AgglomerativeClustering/*.cpp
        AgglomerativeClustering/*.hpp)

file(GLOB ENTITY_SOURCE_FILES
        Entity/*.c
        Entity/*.h)

file(GLOB GENETIC_ALGORITHM_SOURCE_FILES
        GeneticAlgorithm/*.c
        GeneticAlgorithm/*.h
        GeneticAlgorithm/Lib/*.c
        GeneticAlgorithm/Lib/*.h)

set(LIST_SOURCE_FILES
        List/List.c
        List/List.h)

set(LOGGING_SOURCE_FILES
        Logging/Logging.c
        Logging/Logging.h)

set(JOURNAL_SOURCE_FILES
        Journal/Journal.c
        Journal/Journal.h)

set(PLUGINMANAGER_SOURCE_FILES
        PluginManager/PluginManager.c
        PluginManager/PluginManager.h)

set(SPECIES_SOURCE_FILES
        Species/Species.c
        Species/Species.h)

#set(THREADS_SOURCE_FILES
#        Threads/Threads.c
#        Threads/Threads.h)

set(WISHART_SOURCE_FILES
        Wishart/Wishart.c
        Wishart/Wishart.h)

file(GLOB OPTICS_SOURCE_FILES
        OPTICS/*.cpp
        OPTICS/*.h)

set(EVOLUTION_LIB_SOURCE_FILES
        ${ENTITY_SOURCE_FILES}
        ${GENETIC_ALGORITHM_SOURCE_FILES}
        ${LIST_SOURCE_FILES}
        ${LOGGING_SOURCE_FILES}
        ${JOURNAL_SOURCE_FILES}
        ${PLUGINMANAGER_SOURCE_FILES}
        ${SPECIES_SOURCE_FILES}
        ${WISHART_SOURCE_FILES}
        ${AGGLOMERATIVE_CLUSTERING_SOURCE_FILES}
        ${OPTICS_SOURCE_FILES}
        Common.c
        Common.h)

add_library(${PROJECT_NAME} ${EVOLUTION_LIB_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
