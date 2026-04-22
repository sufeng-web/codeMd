cmake_minimum_required(VERSION 3.10)

SET(CMAKE_TOOLCHAIN_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(FileCopy LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)		
set(CMAKE_AUTORCC ON)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 寻找 Qt5 组件
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

add_executable(FileCopy
    main.cpp
    MainWindow.cpp
    MainWindow.h
    FileCopier.cpp
    FileCopier.h
)

# 链接 Qt5 库 (链接 Widgets 通常会自动级联链接 Core)
target_link_libraries(FileCopy PRIVATE Qt5::Widgets Qt5::Core)