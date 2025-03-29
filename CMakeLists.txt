cmake_minimum_required(VERSION 3.10)
project(arcadetui)

set(CMAKE_CXX_STANDARD 11)

find_package(Curses REQUIRED)

include_directories(src)

set(SOURCES
        src/main.cpp
        src/tic_tac_toe.cpp
        src/utils.cpp
)

add_executable(arcadetui ${SOURCES})

target_link_libraries(arcadetui ${CURSES_LIBRARIES})
