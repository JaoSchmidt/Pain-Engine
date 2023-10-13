file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/**/*.cpp")
file(GLOB EXTRA_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(${LIBRARY_NAME} STATIC ${SOURCES} ${EXTRA_SOURCES})
target_compile_options(${LIBRARY_NAME} PRIVATE -Wall -lGLEW -lglfw -lGLU -lGL -lglm)
