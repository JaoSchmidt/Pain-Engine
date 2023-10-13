cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME "Pain Engine")
set(LIBRARY_NAME "pain")
project(pain VERSION 0.1.0.0)

add_compile_definitions(PAIN_PLATFORM_LINUX=1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
add_subdirectory(src)
# =========== Adding Libraries ==================================
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# deactivate tests, examples, doc
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include_directories($(GLEW_INCLUDE_DIRS))
add_subdirectory(external/glfw)

target_link_libraries(${LIBRARY_NAME}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  glfw
)


# =========== Other settings ==================================

# target_include_directories(${TARGET_NAME}
#     PUBLIC
#     ${PROJECT_SOURCE_DIR}/src
# )

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
# message(${HEADER_FILES})
# set_target_properties(${PROJECT_NAME} PROPERTIES 
#   PUBLIC_HEADER ${HEADER_FILES})
# install(TARGETS ${PROJECT_NAME} DESTINATION lib
#   PUBLIC_HEADER DESTINATION include)
