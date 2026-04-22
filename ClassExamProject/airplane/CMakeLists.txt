cmake_minimum_required(VERSION 3.20)
cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${CMAKE_CURRENT_LIST_DIR}/../../../thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE FILEPATH "Vcpkg toolchain file")
endif()
   
project(week04_homework_airplane LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia)

file(GLOB_RECURSE SRC_FILES 
    "src/*.cpp" 
    "src/*.h"
)
add_executable(week04_homework_airplane ${SRC_FILES} resources.qrc)
target_link_libraries(week04_homework_airplane PRIVATE Qt5::Widgets Qt5::Multimedia)


