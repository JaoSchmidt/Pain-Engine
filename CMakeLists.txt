cmake_minimum_required(VERSION 3.25)

project(pingPong VERSION 0.1.0.2)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
# find_package(Qt4 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(src)

# =========== Adding GLFW ==================================
# https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_source

# # Library contained inside submodules
# find_package(Git QUIET)
# if(GIT_FOUND AND EXISTS "${PROJECT_NAME/.git}")
#   option(GIT_SUBMODULE "Check submodules during build" ON)
#   if(GIT_SUBMODULE)
#     message(STATUS "Submodule update")
#     execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive 
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE GIT_SUBMOD_RESULT)
#     if(NOT GIT_SUBMOD_RESULT EQUAL "0")
#       message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}")
#     endif()
#   endif()
# endif()

# # Check all submodules
# if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt")
#   message(FATAL_ERROR "The glfw submodule was not downloaded! GIT_SUBMODULE was turned off or failed")
# endif()

# deactivate tests, examples, doc
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include_directories($(GLEW_INCLUDE_DIRS))
add_subdirectory(external/glfw)
#add_subdirectory(external/glm)

#target_include_directories(${PROJECT_NAME}
  #PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/include
  #  PUBLIC ${PROJECT_SOURCE_DIR}/external/glm
  #) 


target_link_libraries(${PROJECT_NAME}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  glfw
  #  glm
  # qt
)

message("libraries ${GLEW_LIBRARIES}")

# =========== Other settings ==================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(FILES "${PROJECT_BINARY_DIR}/pingPongVersion.h" DESTINATION include)
