cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(PROJECT_NAME "Pain Engine")
set(LIBRARY_NAME "Pain")
project(${LIBRARY_NAME} VERSION 0.1.0)

add_compile_definitions(PAIN_PLATFORM_LINUX=1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(${LIBRARY_NAME} SHARED ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# target_compile_options(${LIBRARY_NAME} PRIVATE -Wall -std=c++20 -fmodules-ts -lGLEW -lglfw -lGLU -lGL -lglm)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
remove_definitions("-DNDEBUG")
target_compile_options(${LIBRARY_NAME} PRIVATE -g -Wall -std=c++17 -lGLEW -lglfw -lGLU -lGL -lglm)

# =========== Adding Libraries ==================================
# deactivate tests, examples, doc
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF)

add_subdirectory(external/glm)
add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)
add_subdirectory(external/glad)
add_subdirectory(external/spdlog)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(OpenGL REQUIRED)

target_link_libraries(${LIBRARY_NAME} PUBLIC
		${OPENGL_LIBRARIES}
		glad
		glm::glm
		SDL2::SDL2
		SDL2_image
		spdlog::spdlog_header_only
)


# =========== Resources/Assets Managment ==================================

# TODO: BUILD THIS FOR WINDOWS
# Define custom target to rsync resources
if(UNIX OR APPLE)
	add_custom_target(sync_resources
		COMMAND rsync -av "${PROJECT_SOURCE_DIR}/resources/" "${CMAKE_BINARY_DIR}/Example/resources/"
		COMMENT "Syncing resources folder"
	)
endif()

# Add dependency to sync_resources target on make
add_dependencies(${LIBRARY_NAME} sync_resources)

# =========== Other settings ==================================

# target_include_directories(${TARGET_NAME}
#     PUBLIC
#     ${PROJECT_SOURCE_DIR}/src
# )

# file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
# message(${HEADER_FILES})
# set_target_properties(${PROJECT_NAME} PROPERTIES 
#   PUBLIC_HEADER ${HEADER_FILES})
# install(TARGETS ${PROJECT_NAME} DESTINATION lib
#   PUBLIC_HEADER DESTINATION include)
