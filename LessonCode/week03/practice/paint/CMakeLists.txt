cmake_minimum_required(VERSION 3.20)

project(week03_practice_paint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(week03_practice_paint WIN32
    main.cpp
)

# 可选：让 MSVC 默认用 UTF-8 读取源码（避免中文字符串/注释乱码）
if (MSVC)
    target_compile_options(week03_practice_paint PRIVATE /utf-8)
endif()

# Win32 API 一般不需要手动链接库；但如果你遇到链接错误，可以打开下面这行：
# target_link_libraries(week03_practice_paint PRIVATE user32 gdi32)