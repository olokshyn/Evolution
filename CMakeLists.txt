cmake_minimum_required(VERSION 3.3)
project(Evolution)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")

include_directories(EvolutionLib)

add_subdirectory(EvolutionLib)

add_subdirectory(EvolutionTests)

set(SOURCE_FILES
        main.c)
add_executable(Evolution ${SOURCE_FILES})
target_link_libraries(Evolution EvolutionLib)

add_dependencies(Evolution EvolutionTests)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(EvolutionLib/Threads REQUIRED)
#target_link_libraries(Evolution Threads::Threads)

target_compile_features(Evolution PRIVATE cxx_range_for)