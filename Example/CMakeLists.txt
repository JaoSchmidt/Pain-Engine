cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME "Painful Game Example")
set(TARGET_NAME "example")
project(${PROJECT_NAME})
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${TARGET_NAME} ${SOURCES})
target_compile_options(${TARGET_NAME} PRIVATE -g -Wall)

target_include_directories(Pain PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/Pain
)
target_link_libraries(${TARGET_NAME} PUBLIC
	SDL2::SDL2
  Pain
)

# =========== Copying Resources ==================================
# Copy all files from resources/ to build directory
file(COPY ${PROJECT_SOURCE_DIR}/resources/ 
	DESTINATION ${CMAKE_BINARY_DIR}/Example/resources/)
