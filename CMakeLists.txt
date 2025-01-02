cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(PROJECT_NAME "Pain Engine")
set(LIBRARY_NAME "Pain")
#set(CMAKE_VERBOSE_MAKEFILE ON)
project(${LIBRARY_NAME} VERSION 0.1.0)

# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/Pain/src/*.cpp")
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(
  ${LIBRARY_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/include/CoreRender/Text>
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
remove_definitions("-DNDEBUG")

target_compile_options(
  ${LIBRARY_NAME}
  PRIVATE -std=c++20 -g -Wall -lGLU -lGL -lglm
)

# =========== Adding Libraries ==================================
# deactivate tests, examples, doc
set(SPDLOG_BUILD_SHARED OFF)

include(FetchContent)

# GLM
message("Fetching glm")
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 47585fde0c49fa77a2bf2fb1d2ead06999fd4b6e
)
FetchContent_MakeAvailable(glm)

message("Bulding glad")
add_subdirectory(Pain/external/glad)

# SPDLOG
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

message("Fetching SDL")
FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.1
)
FetchContent_MakeAvailable(SDL)

message("Fetching SDL_image")
FetchContent_Declare(
  SDL_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG release-2.8.2
)
FetchContent_MakeAvailable(SDL_image)

message("Fetching Imgui")
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
	PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/.github/patches/imgui.patch
	GIT_TAG v1.91.6-docking
	UPDATE_DISCONNECTED 1
)
FetchContent_MakeAvailable(imgui)

# FreeType
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG VER-2-13-3
)
FetchContent_MakeAvailable(freetype)

FetchContent_Declare(
  msdfgen
  GIT_REPOSITORY https://github.com/Chlumsky/msdfgen.git
  GIT_TAG v1.12
)
set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDFGEN_USE_SKIA OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(msdfgen)

# msdf-atlas-gen
FetchContent_Declare(
  msdf-atlas-gen
  GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
	GIT_TAG v1.3
	# UPDATE_DISCONNECTED 1
	# PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/.github/patches/msdf-altas-gen.patch
)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(msdf-atlas-gen)

find_package(OpenGL REQUIRED)

target_link_libraries(
  ${LIBRARY_NAME}
  PUBLIC
    ${OPENGL_LIBRARIES}
    SDL2::SDL2-static
    SDL2_image
    glad
    glm::glm
    spdlog::spdlog_header_only
    imgui
    freetype
    msdf-atlas-gen
)

target_precompile_headers(
  ${LIBRARY_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Pain/include/pch.h
)

# =========== Resources/Assets Managment ==================================

set(GAME_FOLDER "Example/textRendering")

# TODO: BUILD THIS FOR WINDOWS AND TEST IT
# Define custom target to rsync resources
if(UNIX OR APPLE)
  add_custom_target(
    sync_resources
    COMMAND
      rsync -av "${PROJECT_SOURCE_DIR}/Pain/resources/"
      "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources/"
    # For debuging purposes
    COMMAND
      rsync -av "${PROJECT_SOURCE_DIR}/Pain/resources/"
      "${PROJECT_SOURCE_DIR}/resources/"
    COMMENT "Syncing resources folder"
  )
  # Add dependency to sync_resources target on make
  add_dependencies(${LIBRARY_NAME} sync_resources)
else()
  add_custom_target(
    sync_resources
    COMMAND
      Copy-Item -Path "${PROJECT_SOURCE_DIR}\\Pain\\resources\\*" -Destination
      "${CMAKE_BINARY_DIR}\\${GAME_FOLDER}\\resources\\" -Recurse
    COMMENT "Syncing resources folder"
  )

  add_dependencies(${LIBRARY_NAME} sync_resources)
endif()

add_subdirectory(${GAME_FOLDER})
