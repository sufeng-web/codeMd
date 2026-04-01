cmake_minimum_required(VERSION 3.10)
project(Memory_application_scenarios CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
   main.cpp
)
set(CMAKE_CXX_STANDARD 11)
add_executable(MAS  ${SOURCES} )
set_target_properties(MAS  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Debug Practice: use Debug config and set breakpoints to practice.")
