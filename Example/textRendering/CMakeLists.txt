cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Painful text")
project(${PROJECT_NAME})
set(TARGET_NAME "textRender")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${TARGET_NAME} ${SOURCES})
target_compile_options(${TARGET_NAME} PRIVATE -g -Wall)

target_include_directories(Pain PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/Pain
)

target_link_libraries(${TARGET_NAME} PUBLIC
  Pain
)

# =========== Copying Resources ==================================
# Copy all files from resources/ to build directory
message(${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/resources/ 
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# For debuging purposes
file(COPY ${PROJECT_SOURCE_DIR}/resources/ 
	DESTINATION ${CMAKE_SOURCE_DIR}/resources/)
