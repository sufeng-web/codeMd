cmake_minimum_required(VERSION 3.20)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE FILEPATH "Vcpkg toolchain file")
endif()
project(ClassExamProject LANGUAGES CXX)

message("ClassExamProject dir")
add_subdirectory(airplane)





