cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Painful Script Game Example")
project(${PROJECT_NAME})
set(TARGET_NAME "scriptGame")

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${TARGET_NAME} ${SOURCES})
target_compile_options(${TARGET_NAME} PRIVATE -g -Wall)

target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_include_directories(
  Pain
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/Pain
)

target_link_libraries(${TARGET_NAME} PUBLIC Pain)

# =========== Copying Resources ==================================
# Copy all files from resources/ to build directory
# file(COPY ${PROJECT_SOURCE_DIR}/resources/ 
# 	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# TODO: BUILD THIS FOR WINDOWS AND TEST IT
# Define custom target to rsync resources
if(UNIX OR APPLE)
  add_custom_target(
    sync_resources_game
    COMMAND
      rsync -av "${PROJECT_SOURCE_DIR}/resources/"
      "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources/"
    COMMENT "Syncing resources folder"
  )
  # Add dependency to sync_resources target on make
  add_dependencies(${LIBRARY_NAME} sync_resources_game)
else()
  add_custom_target(
    sync_resources
    COMMAND
      Copy-Item -Path "${PROJECT_SOURCE_DIR}\\resources\\*" -Destination
      "${CMAKE_BINARY_DIR}\\${GAME_FOLDER}\\resources\\" -Recurse
    COMMENT "Syncing resources folder"
  )

  add_dependencies(${LIBRARY_NAME} sync_resources_game)
endif()
