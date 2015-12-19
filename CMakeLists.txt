cmake_minimum_required(VERSION 3.3)
project(Wishart)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES main.c Wishart/Wishart.h Wishart/Wishart.c List/List.c List/List.h List/ListTests.c List/ListTests.h Wishart/WishartTest.c Wishart/WishartTest.c Wishart/WishartTest.h)
add_executable(Wishart ${SOURCE_FILES})