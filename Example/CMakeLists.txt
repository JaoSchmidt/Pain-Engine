cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME "Painful Game Example")
set(TARGET_NAME "example")
project(${PROJECT_NAME})
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_subdirectory(src)

target_include_directories(pain PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/Pain/src
)

target_link_libraries(${TARGET_NAME} PUBLIC
  pain
)

# =========== Other settings ==================================

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
