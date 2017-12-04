project(Console)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-function -Werror")

include_directories(../EvolutionLib)

set(SOURCE_FILES
        main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} EvolutionLib)
