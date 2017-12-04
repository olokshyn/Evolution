project(EvolutionTests)

add_subdirectory(googletest)

include_directories(../EvolutionLib)
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
set(OPTICS_TEST_SOURCE_FILES
        OPTICS/OpticsTests.cpp)

set(EVOLUTION_TESTS_SOURCE_FILES
        ${AGGLOMERATIVE_CLUSTERING_TEST_SOURCE_FILES}
        ${ENTITY_TEST_SOURCE_FILES}
        ${LIST_TEST_SOURCE_FILES}
        ${SPECIES_TEST_SOURCE_FILES}
        ${WISHART_TEST_SOURCE_FILES}
        ${OPTICS_TEST_SOURCE_FILES}
        TestsCommon.cpp
        TestsCommon.h)
add_executable(${PROJECT_NAME} ${EVOLUTION_TESTS_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} gtest_main EvolutionLib)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Wishart/data
        ${CMAKE_CURRENT_BINARY_DIR}/data
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Wishart/data)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${PROJECT_NAME} --gtest_filter=-*FCPS*)
