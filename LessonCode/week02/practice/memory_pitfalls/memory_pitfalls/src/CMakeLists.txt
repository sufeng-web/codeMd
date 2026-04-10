cmake_minimum_required(VERSION 3.10)
project(week02_practice_memorypitfalls LANGUAGES CXX)

# Build dir as sibling of src: from memory_pitfalls run: cmake -B build -S src
set(CMAKE_CXX_STANDARD 17)

# Coding style: source filenames lowercase, no underscore; class/struct member variables prefixed with m_
# Real scenario only: ThemeLoader, ConfigEntry, ThemeNameView, ConfigValueView, ApplyHandler, FileApplyHandler, LegacyBridge

set(SOURCES
    main.cpp
    guardalloc.cpp
    themeloader.cpp
    applyhandler.cpp
    legacybridge.cpp
    themeconfig.cpp
    themedisplay.cpp
    exportcontext.cpp
    previewreader.cpp
    handlerfactory.cpp
)
add_executable(week02_practice_memorypitfalls ${SOURCES})
set_target_properties(week02_practice_memorypitfalls PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
target_include_directories(week02_practice_memorypitfalls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(MSVC)
    target_compile_options(week02_practice_memorypitfalls PRIVATE
        $<$<CONFIG:Release>:/Zi>)
    target_link_options(week02_practice_memorypitfalls PRIVATE
        $<$<CONFIG:Release>:/DEBUG>)
    target_link_libraries(week02_practice_memorypitfalls PRIVATE dbghelp)
elseif(WIN32)
    target_link_libraries(week02_practice_memorypitfalls PRIVATE dbghelp)
endif()

message(STATUS "src: theme config scenario (6-7 classes only).")
