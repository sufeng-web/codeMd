cmake_minimum_required(VERSION 3.10)
project(week01_practice_score_analyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    score_analyzer.cpp
    score_reader.cpp
    name_reader.cpp
    statistics.cpp
    report.cpp
    globals.cpp
)
add_executable(week01_practice_score_analyzer ${SOURCES})
set_target_properties(week01_practice_score_analyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Debug Practice: use Debug config and set breakpoints to practice.")
