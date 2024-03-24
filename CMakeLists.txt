cmake_minimum_required(VERSION 3.25)

project(topproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(Pain/external/SDL)
add_subdirectory(Pain)
add_subdirectory(Example)
