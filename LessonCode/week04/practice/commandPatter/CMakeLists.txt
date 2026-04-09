# CMake最低版本（匹配你老师的要求）
cmake_minimum_required(VERSION 3.10)

# 项目名称（可自定义）
project(Week04_practice_commandPattern)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动将可执行文件输出到 build/bin 目录（适配你的总构建结构）
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# 编译生成可执行文件（源文件是 main.cpp）
add_executable(week04_practice_commandPatter main.cpp)