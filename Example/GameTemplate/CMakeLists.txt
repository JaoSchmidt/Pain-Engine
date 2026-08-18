# Root CMakeLists.txt for Pain Engine Game Projects This file is part of the
# Pain Engine repository. Your game lives inside Example/ alongside other games.
#
# Project Structure: Pain-Engine/ ├── Pain/                  (Engine source) └──
# Example/ └── GameTemplate/      (this file) ├── CMakeLists.txt ├── src/ (your
# game source files) ├── include/       (your game headers) └── resources/ (your
# game assets)

cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Unnamed Project")
project(${PROJECT_NAME} VERSION 0.1.0)

set(TARGET_NAME "myexecutable")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =========== Game Executable Configuration ==================================

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
     "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${TARGET_NAME} ${GAME_SOURCES})

# Include directories for your game
target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# C++ Standard Configuration
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 20
                                                CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /std:c++20 /Zi)
  set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -g -std=c++20 -Wall -Wextra)
endif()

# =========== Pain Engine Integration ==================================
# Link the engine library to your game executable
target_link_libraries(${TARGET_NAME} PUBLIC Pain)

# =========== Resources Management ==================================
# Define the resources directory for your game
set(RESOURCES_SOURCE_DIR "${PROJECT_SOURCE_DIR}/resources")
set(RESOURCES_DEST_DIR "${CMAKE_BINARY_DIR}/resources")

# Copy resources to build directory (cross-platform)
if(UNIX OR APPLE)
  add_custom_target(
    copy_game_resources_${TARGET_NAME} ALL
    COMMAND rsync -av "${RESOURCES_SOURCE_DIR}/" "${RESOURCES_DEST_DIR}/"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Syncing game resources to build directory")
else()
  add_custom_target(
    copy_game_resources_${TARGET_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCES_SOURCE_DIR}"
            "${RESOURCES_DEST_DIR}"
    COMMENT "Copying game resources to build directory")
endif()

# Ensure resources are copied before building the executable
add_dependencies(${TARGET_NAME} copy_game_resources_${TARGET_NAME})

# =========== Build Information Banner ==================================
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S")

if(UNIX)
  add_custom_target(
    BuildBanner ALL
    COMMAND
      printf
      "\\n\\033[1;32m==========================================================================================\\n"
    COMMAND printf "  PROJECT:   ${PROJECT_NAME}\\n"
    COMMAND printf "  TARGET:    ${TARGET_NAME}\\n"
    COMMAND printf "  VERSION:   ${PROJECT_VERSION}\\n"
    COMMAND printf "  TYPE:      ${CMAKE_BUILD_TYPE}\\n"
    COMMAND printf "  STANDARD:  C++${CMAKE_CXX_STANDARD}\\n"
    COMMAND
      printf
      "  COMPILER:  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\\n"
    COMMAND printf "  SOURCE:    ${CMAKE_SOURCE_DIR}\\n"
    COMMAND printf "  BINARY:    ${CMAKE_BINARY_DIR}\\n"
    COMMAND printf "  DATE:      ${BUILD_TIME}\\n"
    COMMAND
      printf
      "==========================================================================================\\033[0m\\n\\n"
    VERBATIM)
endif()
