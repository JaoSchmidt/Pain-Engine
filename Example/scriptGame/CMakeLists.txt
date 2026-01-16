cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Painful Script Game Example")
project(${PROJECT_NAME})
set(TARGET_NAME "scriptGame")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${TARGET_NAME} ${SOURCES})

# setting c++20 for the executable
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 20
                                                CXX_STANDARD_REQUIRED ON)
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /std:c++20 /Zi)
  set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -g -std=c++20 -Wall)
endif()

target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_include_directories(Pain
                           PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/Pain)

target_link_libraries(${TARGET_NAME} PUBLIC Pain)

# =========== Copying Resources ==================================
# Copy all files from resources/ to build directory file(COPY
# ${PROJECT_SOURCE_DIR}/resources/ DESTINATION
# ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# TODO: BUILD THIS FOR WINDOWS AND TEST IT Define custom target to rsync
# resources
if(UNIX OR APPLE)
  add_custom_target(
    sync_resources_game
    COMMAND rsync -av "${PROJECT_SOURCE_DIR}/resources/"
            "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources/"
    COMMENT "Syncing resources folder")
  # Add dependency to sync_resources target on make
  add_dependencies(${LIBRARY_NAME} sync_resources_game)
else()
  add_custom_target(
    sync_resources_game
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources"
            "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources"
    COMMENT "Syncing resources folder")

  add_dependencies(${LIBRARY_NAME} sync_resources_game)
endif()
