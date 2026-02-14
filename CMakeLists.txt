# CMAKELists.txt
cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(PROJECT_NAME "Pain Engine")
set(LIBRARY_NAME "Pain")
set(MACRO_DEFINITIONS "PAIN_RENDERER_OPENGL")
project(${PROJECT_NAME} VERSION 0.1.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     "${PROJECT_SOURCE_DIR}/Pain/src/*.cpp",
     "${PROJECT_SOURCE_DIR}/Pain/platform/*.cpp")
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

# file(GLOB_RECURSE MODULES "Pain/include/*.cppm") message("Finding the
# modules:")

# foreach(source ${MODULES}) message("- ${source}") endforeach() target_sources(
# ${LIBRARY_NAME} PUBLIC FILE_SET CXX_MODULES FILES ${MODULES} PRIVATE
# ${SOURCES} )
set(BUILD_SHARED_LIBS OFF)

target_include_directories(
  ${LIBRARY_NAME}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/include>
         $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/platform>
         $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/include/CoreRender/Text>
)
set_target_properties(${LIBRARY_NAME} PROPERTIES CXX_STANDARD 20
                                                 CXX_STANDARD_REQUIRED ON)
target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  remove_definitions("-DNDEBUG")
  target_compile_options(
    ${LIBRARY_NAME}
    PRIVATE -Wall
            -g
            -O0
            -Wextra
            -Wconversion
            -Wno-unused-parameter
            -Wmissing-field-initializers
            -Wno-sign-conversion
            -fdiagnostics-color=always
            -std=c++20
            # specifc to gcc -fmodules-ts
            -Wpessimizing-move)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL
                                                "Release")
  add_definitions("-DNDEBUG")
  target_compile_options(
    ${LIBRARY_NAME}
    PRIVATE -Wall
            -O3
            -Wextra
            -Wpedantic
            -Wconversion
            -Wno-unused-parameter
            -Wmissing-field-initializers
            -Wno-sign-conversion
            -fdiagnostics-color=always
            -std=c++20
            # specifc to gcc -fmodules-ts
            -Wpessimizing-move)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL
                                                  "Debug")
  remove_definitions("-DNDEBUG")
  target_compile_options(
    ${LIBRARY_NAME}
    PRIVATE -Wall
            -g
            -O0
            -Wextra
            -Wpedantic
            -Wconversion
            # -Wno-unused-parameter -Wmissing-field-initializers
            # -Wno-sign-conversion -fmodules -fmodules-cache-path=./module-cache
            -ftemplate-backtrace-limit=0
            -fcolor-diagnostics
            -std=c++20
            # specific to clang
            -Wreturn-std-move
            -Wself-move)
elseif(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  remove_definitions("-DNDEBUG")
  target_compile_options(
    ${LIBRARY_NAME}
    PUBLIC /Zc:preprocessor # Required for __VA_OPT__
           /utf-8 # Set source and execution character sets to UTF-8
  )
  target_compile_options(
    ${LIBRARY_NAME}
    PRIVATE /W4 # Warning level 4 (high)
            /WX- # Don't treat warnings as errors
            # /permissive- # Standards conformance mode
            /Od # Disable optimization
            /MP # Multi-processor compilation
            /EHsc # Exception handling model
            /Zi # Generate debug info
            /std:c++20 # C++20 standard
  )
  set_target_properties(${LIBRARY_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
else()
  target_compile_options(
    ${LIBRARY_NAME} PRIVATE -std=c++20 -Wall # -lGLU -lGL -lglm -lm
                                             # -fdiagnostics-all-candidates
  )
endif()

# =========== Adding Libraries ==================================
# deactivate tests, examples, doc
set(SPDLOG_BUILD_SHARED OFF)
include(FetchContent)

# mINI
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/mINI/CMakeLists.txt")
  message("Fetching mINI")
  FetchContent_Declare(mINI GIT_REPOSITORY https://github.com/metayeti/mINI.git)
  FetchContent_MakeAvailable(mINI)
  target_include_directories(
        ${LIBRARY_NAME}
        SYSTEM PUBLIC
        ${mINI_SOURCE_DIR}
  )
else()
  message("Using submodule for mINI")
  add_subdirectory(Pain/external/mINI)
  target_include_directories(
      ${LIBRARY_NAME}
      SYSTEM PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/external/mINI>
  )
endif()

# GLM
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/glm/CMakeLists.txt")
  message("Fetching glm")
  FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 47585fde0c49fa77a2bf2fb1d2ead06999fd4b6e)
  FetchContent_MakeAvailable(glm)
else()
  message("Using submodule for glm")
  add_subdirectory(Pain/external/glm)
endif()

message("Bulding glad")
add_subdirectory(Pain/external/glad)

# SPDLOG
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/spdlog/CMakeLists.txt")
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.2)
  FetchContent_MakeAvailable(spdlog)
else()
  message("Using submodule for spdlog")
  add_subdirectory(Pain/external/spdlog)
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/SDL/CMakeLists.txt")
  message("Fetching SDL")
  FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG 6be87ceb33a9aad3bf5204bb13b3a5e8b498fd26 # release-2.32.2 fixed
  )
  FetchContent_MakeAvailable(SDL)
else()
  message("Using submodule for SDL")
  add_subdirectory(Pain/external/SDL)
endif()

# perlin noise
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/PerlinNoise/.git")
  message("Fetching PerlinNoise")
  FetchContent_Declare(
    perlin
    GIT_REPOSITORY https://github.com/Reputeless/PerlinNoise.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(perlin)
  target_include_directories(
      ${LIBRARY_NAME}
      SYSTEM PUBLIC
      ${perlin_SOURCE_DIR}
  )
else()
  message("Using submodule for PerlinNoise")
  target_include_directories(
    ${LIBRARY_NAME} SYSTEM PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/external/PerlinNoise>
  )
endif()

# stb
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/stb/.git")
  message("Fetching stb")
  FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(stb)
  target_include_directories(
      ${LIBRARY_NAME}
      SYSTEM PUBLIC
      ${stb_SOURCE_DIR}
  )
else()
  message("Using submodule for stb")
  target_include_directories(
    ${LIBRARY_NAME} SYSTEM
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/external/stb>
  )
endif()

# IMGUI
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/imgui/.git")
  message("Fetching Imgui")
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.92.5-docking
  )
  FetchContent_MakeAvailable(imgui)

  add_library(imgui STATIC
      ${imgui_SOURCE_DIR}/imgui.cpp
      ${imgui_SOURCE_DIR}/imgui_draw.cpp
      ${imgui_SOURCE_DIR}/imgui_tables.cpp
      ${imgui_SOURCE_DIR}/imgui_widgets.cpp
      ${imgui_SOURCE_DIR}/imgui_demo.cpp
      ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
      ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  )

  target_include_directories(imgui PUBLIC
      ${imgui_SOURCE_DIR}
      ${imgui_SOURCE_DIR}/backends
  )
  target_link_libraries(imgui PRIVATE SDL2::SDL2)
else()
  message("Using submodule for imgui")
  set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/imgui)
  add_library(imgui STATIC
      ${IMGUI_DIR}/imgui.cpp
      ${IMGUI_DIR}/imgui_draw.cpp
      ${IMGUI_DIR}/imgui_tables.cpp
      ${IMGUI_DIR}/imgui_widgets.cpp
      ${IMGUI_DIR}/imgui_demo.cpp
      ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
      ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  )
  target_include_directories(imgui PUBLIC
      ${IMGUI_DIR}
      ${IMGUI_DIR}/backends
  )
  target_link_libraries(imgui PRIVATE SDL2::SDL2)
  # target_sources(
  #   imgui
  #   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/imgui/misc/cpp/imgui_stdlib.cpp)

endif()

# msdf-atlas-gen
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "" FORCE)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/msdf-atlas-gen/.git")
  message("Fetching msdf-atlas-gen")
  FetchContent_Declare(
    msdf-atlas-gen
    GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
    GIT_TAG c76a32319934c39e51a8c4838240d7b2362b0882
    GIT_SUBMODULES_RECURSE TRUE   
  )
  FetchContent_MakeAvailable(msdf-atlas-gen)
else()
  message("Using submodule for msdf-atlas-gen")
  add_subdirectory("Pain/external/msdf-atlas-gen")
endif()

# Lua
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/Lua/CMakeLists.txt")
  message("Fetching Lua")
  FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/walterschell/Lua.git
    GIT_TAG v5.4.7)
  FetchContent_MakeAvailable(lua)
else()
  message("Using submodule for Lua")
  add_subdirectory("Pain/external/Lua")
endif()

# Sol2
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Pain/external/sol2/.git")
  message("Fetching Sol2")
  FetchContent_Declare(
    sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.5.0)
  FetchContent_MakeAvailable(sol2)
else()
  message("Using submodule for sol2")
  add_subdirectory("Pain/external/sol2")
endif()

find_package(OpenGL REQUIRED)

# SURPRESS WARNINGS INSIDE SOME LIBRARIES
set_target_properties(
  msdf-atlas-gen
  PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
             $<TARGET_PROPERTY:msdf-atlas-gen,INTERFACE_INCLUDE_DIRECTORIES>)
set_target_properties(
  mINI PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                  $<TARGET_PROPERTY:mINI,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(
  ${LIBRARY_NAME}
  PUBLIC ${CMAKE_DL_LIBS}
         ${OPENGL_LIBRARIES}
         SDL2::SDL2-static
         lua_static
         glad
         mINI
         glm::glm
         spdlog::spdlog_header_only
         imgui
         msdf-atlas-gen
         sol2::sol2)

target_precompile_headers(${LIBRARY_NAME} PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}/Pain/include/pch.h)

# =========== Resources/Assets Managment ==================================

set(GAME_FOLDER "Example/scriptGame")
add_compile_definitions(${MACRO_DEFINITIONS})

# TODO: BUILD THIS FOR WINDOWS AND TEST IT Define custom target to rsync
# resources
if(UNIX OR APPLE)
  add_custom_target(
    sync_resources
    COMMAND rsync -av "${PROJECT_SOURCE_DIR}/Pain/resources/"
            "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources/"
    COMMAND
      rsync -av "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources/"
      "${PROJECT_SOURCE_DIR}/resources/"
      # NOTE: not sure if this is necessasry but I'm just going to assume for
      # now that every developer will need to sync the lua types inside the
      # project source directory. Why? beacuse lua lsp works with these files
    COMMAND rsync -av "${PROJECT_SOURCE_DIR}/Pain/resources/scripts/"
            "${PROJECT_SOURCE_DIR}/"
    COMMENT "Syncing resources folder")
  # Add dependency to sync_resources target on make
  add_dependencies(${LIBRARY_NAME} sync_resources)
else()
  add_custom_target(
    sync_resources
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/Pain/resources"
      "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources"
    COMMENT "Syncing resources folder")

  add_dependencies(${LIBRARY_NAME} sync_resources)
endif()

# =========== Dump shit to separete the compiler vomiting information ======
# get current date (every build) set(COLOR_GREEN "\x1b[1;32m") set(COLOR_BLUE
# "\x1b[1;34m") set(COLOR_RESET "\x1b[0m") string(TIMESTAMP NOW "%Y-%m-%d
# %H:%M:%S")

string(TIMESTAMP NOW "%Y-%m-%d %H:%M:%S")

if(UNIX)
  add_custom_target(
    BuildInitialBanner ALL
    COMMAND
      printf
      "\\n\\033[1;32m==========================================================================================\\n"
    COMMAND printf "  BUILD:   ${PROJECT_NAME}\\n"
    COMMAND printf "  VERSION:   ${PROJECT_VERSION}\\n"
    COMMAND printf "  TYPE:    ${CMAKE_BUILD_TYPE}\\n"
    COMMAND printf "  STANDARD:  C++${CMAKE_CXX_STANDARD}\\n"
    COMMAND
      printf
      "  COMPILER: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\\n"
    COMMAND printf "  SOURCE:    ${CMAKE_CURRENT_SOURCE_DIR}\\n"
    COMMAND printf "  BINARY:    ${CMAKE_BINARY_DIR}\\n"
    COMMAND printf "  DATE:    ${NOW}\\n"
    COMMAND
      printf
      "==========================================================================================\\033[0m\\n\\n"
    VERBATIM)
endif()

# =========================================================================
add_subdirectory(${GAME_FOLDER})
