include_directories(${gtest_SOURCE_DIR}/include)

set(AGGLOMERATIVE_CLUSTERING_TEST_SOURCE_FILES
        AgglomerativeClustering/ClusterTests.cpp)

set(EVOLUTION_TESTS_SOURCE_FILES
        ${AGGLOMERATIVE_CLUSTERING_TEST_SOURCE_FILES})
add_executable(EvolutionTests ${EVOLUTION_TESTS_SOURCE_FILES})
target_link_libraries(EvolutionTests gtest_main)
target_link_libraries(EvolutionTests EvolutionLib)

add_custom_command(
        TARGET EvolutionTests
        POST_BUILD
        COMMAND EvolutionTests)