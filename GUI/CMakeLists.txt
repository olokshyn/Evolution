cmake_minimum_required(VERSION 2.8)

project(GUI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Werror -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package (Threads)

include_directories(EvolutionLib)
add_subdirectory(EvolutionLib)
# add_subdirectory(EvolutionTests)

add_executable(${PROJECT_NAME}
        Resources/resources.qrc
        main.cpp
        IterationInfo.h
        MainWindow/MainWindow.cpp
        MainWindow/MainWindow.h
        EvolutionWidget/EvolutionWidget.cpp
        EvolutionWidget/EvolutionWidget.h
        EvolutionWidget/EvolutionWorker.cpp
        EvolutionWidget/EvolutionWorker.h
        ToolsWidgets/SettingsWidget.cpp
        ToolsWidgets/SettingsWidget.h
        ToolsWidgets/LauncherWidget.cpp
        ToolsWidgets/LauncherWidget.h
        Utils/Utils.cpp
        Utils/Utils.h)

target_link_libraries(${PROJECT_NAME} EvolutionLib ${CMAKE_THREAD_LIBS_INIT})
qt5_use_modules(${PROJECT_NAME} Widgets Charts)

# add_dependencies(${PROJECT_NAME} EvolutionTests)
