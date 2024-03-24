cmake_minimum_required(VERSION 3.25)

project(topproject)

set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL" CACHE PATH "Path to SDL2 library")
find_package(SDL2 REQUIRED)

add_subdirectory(Pain)
add_subdirectory(Example)
