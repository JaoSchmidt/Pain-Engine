cmake_minimum_required(VERSION 3.6)

set(LIBRARY_NAME "glad")
project(${LIBRARY_NAME})

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")
message(${PROJECT_SOURCE_DIR})

add_library(${LIBRARY_NAME} SHARED ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
