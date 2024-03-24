cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(PROJECT_NAME "Pain Engine")
set(LIBRARY_NAME "Pain")
project(${LIBRARY_NAME} VERSION 0.1.0)

# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
# STATIC?
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
remove_definitions("-DNDEBUG")
#target_compile_options(${LIBRARY_NAME} PRIVATE -g -Wall /std:c++17 -lGLU -lGL -lglm)
if(MSVC)
  target_compile_options(${LIBRARY_NAME} PRIVATE /W0)
else()
  target_compile_options(${LIBRARY_NAME} PRIVATE -Wall)
endif()

# =========== Adding Libraries ==================================
# deactivate tests, examples, doc
set(SPDLOG_BUILD_SHARED OFF)

add_subdirectory(external/glm)
add_subdirectory(external/glad)
add_subdirectory(external/spdlog)
add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)


find_package(OpenGL REQUIRED)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	SDL2
	SDL2::SDL2main
	SDL2_image
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
	${OPENGL_LIBRARIES}
	glad
	glm::glm
	spdlog::spdlog_header_only
)

# target_link_libraries(${LIBRARY_NAME} PRIVATE
# 	SDL2_image
# )

# # libraries that have .cpp files must also be included this way:
# include_directories(PUBLIC 
# 	"${PROJECT_SOURCE_DIR}/external/SDL/include"
# )

# =========== Resources/Assets Managment ==================================

# TODO: BUILD THIS FOR WINDOWS
# Define custom target to rsync resources
if(UNIX OR APPLE)
	add_custom_target(sync_resources
		COMMAND rsync -av "${PROJECT_SOURCE_DIR}/resources/" "${CMAKE_BINARY_DIR}/Example/resources/"
		COMMENT "Syncing resources folder"
	)
	# Add dependency to sync_resources target on make
	add_dependencies(${LIBRARY_NAME} sync_resources)
endif()
