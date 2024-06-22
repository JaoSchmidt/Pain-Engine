cmake_minimum_required(VERSION 3.25)

project(topproject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(PROJECT_NAME "Pain Engine")
set(LIBRARY_NAME "Pain")
#set(CMAKE_VERBOSE_MAKEFILE ON)
project(${LIBRARY_NAME} VERSION 0.1.0)

# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/Pain/src/*.cpp")
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Pain/include>
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
remove_definitions("-DNDEBUG")

target_compile_options(${LIBRARY_NAME} PRIVATE -std=c++20 -g -Wall -lGLU -lGL -lglm)

# =========== Adding Libraries ==================================
# deactivate tests, examples, doc
set(SPDLOG_BUILD_SHARED OFF)

add_subdirectory(Pain/external/glm)
add_subdirectory(Pain/external/glad)
add_subdirectory(Pain/external/spdlog)
add_subdirectory(Pain/external/SDL)
add_subdirectory(Pain/external/SDL_image)
find_package(OpenGL REQUIRED)

target_link_libraries(${LIBRARY_NAME} PUBLIC
	${OPENGL_LIBRARIES}
	SDL2_image
	SDL2::SDL2
	glad
	glm::glm
	spdlog::spdlog_header_only
)

target_precompile_headers(${LIBRARY_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Pain/include/pch.h
)

# =========== Resources/Assets Managment ==================================

set(GAME_FOLDER "Example/spriteGame")

# TODO: BUILD THIS FOR WINDOWS AND TEST IT
# Define custom target to rsync resources
if(UNIX OR APPLE)
	add_custom_target(sync_resources
		COMMAND rsync -av "${PROJECT_SOURCE_DIR}/Pain/resources/" "${CMAKE_BINARY_DIR}/${GAME_FOLDER}/resources/"
		COMMENT "Syncing resources folder"
	)
	# Add dependency to sync_resources target on make
	add_dependencies(${LIBRARY_NAME} sync_resources)
else()
	add_custom_target(sync_resources
		COMMAND Copy-Item -Path "${PROJECT_SOURCE_DIR}\\Pain\\resources\\*" -Destination "${CMAKE_BINARY_DIR}\\${GAME_FOLDER}\\resources\\" -Recurse
		COMMENT "Syncing resources folder"
	)

	add_dependencies(${LIBRARY_NAME} sync_resources)
endif()

add_subdirectory(${GAME_FOLDER})
