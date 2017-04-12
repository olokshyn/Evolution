add_subdirectory(googletest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include)

set(AGGLOMERATIVE_CLUSTERING_TEST_SOURCE_FILES
        AgglomerativeClustering/ClusterTests.cpp)
set(ENTITY_TEST_SOURCE_FILES
        Entity/EntityTests.cpp
        Entity/CrossoverTests.cpp)
set(LIST_TEST_SOURCE_FILES
        List/ListTests.cpp)
set(SPECIES_TEST_SOURCE_FILES
        Species/SpeciesTests.cpp)
set(WISHART_TEST_SOURCE_FILES
        Wishart/WishartTests.cpp)

set(EVOLUTION_TESTS_SOURCE_FILES
        ${AGGLOMERATIVE_CLUSTERING_TEST_SOURCE_FILES}
        ${ENTITY_TEST_SOURCE_FILES}
        ${LIST_TEST_SOURCE_FILES}
        ${SPECIES_TEST_SOURCE_FILES}
        ${WISHART_TEST_SOURCE_FILES}
        TestsCommon.cpp
        TestsCommon.h)
add_executable(EvolutionTests ${EVOLUTION_TESTS_SOURCE_FILES})
target_link_libraries(EvolutionTests gtest_main)
target_link_libraries(EvolutionTests EvolutionLib)

add_custom_command(
        TARGET EvolutionTests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/Wishart/FisherIris.txt
        ${CMAKE_CURRENT_BINARY_DIR}/FisherIris.txt
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Wishart/FisherIris.txt)

#add_custom_command(
#        TARGET EvolutionTests
#        POST_BUILD
#        COMMAND EvolutionTests)
