cmake_minimum_required(VERSION 3.15)
project(kolibri_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)


qt_add_executable(kolibri_test main.cpp
                src/mainwindow.cpp
                src/mainwindow.ui
                src/worker.cpp)
target_link_libraries(kolibri_test PRIVATE Qt6::Widgets Qt6::Core)
target_include_directories(kolibri_test PRIVATE include)

#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/065127f1e4b46c5f14fc73cf8d323c221f9dc68e.zip
#        DOWNLOAD_EXTRACT_TIMESTAMP true
#)

