cmake_minimum_required(VERSION 3.20)
cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${CMAKE_CURRENT_LIST_DIR}/../thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE FILEPATH "Vcpkg toolchain file")
endif()

project(airplane LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
add_executable(airplane src/main.cpp src/gui/MainMenu.h src/gui/MainMenu.cpp src/games/apple/AppleGame.h src/games/apple/AppleGame.cpp src/games/apple/AppleSettingsDialog.h src/games/apple/AppleSettingsDialog.cpp resources.qrc)
target_link_libraries(airplane PRIVATE Qt5::Widgets)


