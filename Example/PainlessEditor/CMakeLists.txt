cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Painless Editor")
project(${PROJECT_NAME})
set(TARGET_NAME "Painless")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(${TARGET_NAME} ${SOURCES})

# setting c++20 for the executable set_target_properties(${TARGET_NAME}
# PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
# target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /std:c++20 /Zi)
  set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -g -std=c++20 -Wall)
endif()

target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_include_directories(Pain PRIVATE ${ENGINE_DIRECTORY}/Pain/include)

# IMGUI
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/.git")
  message(
    "Fetching Imgui because ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/.git, doesn't exist"
  )
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.92.5-docking)
  FetchContent_MakeAvailable(imgui)

  add_library(
    imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

  target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}
                                          ${imgui_SOURCE_DIR}/backends)
  target_link_libraries(imgui PRIVATE SDL2::SDL2)
  target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)
else()
  message("Using submodule for imgui from ${CMAKE_CURRENT_SOURCE_DIR}")
  set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
  add_library(
    imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
  target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
  target_link_libraries(imgui PRIVATE SDL2::SDL2)
  target_sources(imgui PRIVATE ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp)

endif()

# implot
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/implot/.git")
  message(
    "Fetching Implot because ${CMAKE_CURRENT_SOURCE_DIR}/external/implot/.git, doesn't exist"
  )
  FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.17)
  FetchContent_MakeAvailable(implot)

  add_library(
    implot STATIC
    ${implot_SOURCE_DIR}/implot.h #
    ${implot_SOURCE_DIR}/implot.cpp #
    ${implot_SOURCE_DIR}/implot_internal.h #
    ${implot_SOURCE_DIR}/implot_items.cpp
    ${implot_SOURCE_DIR}/implot_demo.cpp)
  target_link_libraries(implot PRIVATE imgui)
  target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
else()
  message("Using submodule for imgui from ${CMAKE_CURRENT_SOURCE_DIR}")
  set(implot_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/implot)
  add_library(
    implot STATIC
    ${implot_DIR}/implot.h #
    ${implot_DIR}/implot.cpp #
    ${implot_DIR}/implot_internal.h #
    ${implot_DIR}/implot_items.cpp #
    ${implot_DIR}/implot_demo.cpp)
  target_link_libraries(implot PRIVATE imgui)
  target_include_directories(implot PUBLIC ${implot_DIR})
endif()

target_link_libraries(
  ${TARGET_NAME}
  PUBLIC ${CMAKE_DL_LIBS} #
         ${OPENGL_LIBRARIES} #
         imgui #
         implot #
)

target_link_libraries(${TARGET_NAME} PRIVATE Pain)
