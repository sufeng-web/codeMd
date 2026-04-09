cmake_minimum_required(VERSION 3.10)
project(week02_homework_MAS CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
   main.cpp
)
set(CMAKE_CXX_STANDARD 11)
add_executable(week02_homework_MAS  ${SOURCES} )
set_target_properties(week02_homework_MAS  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Debug Practice: use Debug config and set breakpoints to practice.")
