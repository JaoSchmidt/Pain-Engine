
# include_directories(${MyProject_SOURCE_DIR}/src)

# add_executable(${PROJECT_NAME} app-tutorial.cpp)
add_executable(${PROJECT_NAME} main.cpp)
# target_compile_options(${PROJECT_NAME} PRIVATE -Wall -lglfw3 -lGLU -lGL)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -lGLEW -lglfw -lGLU -lGL -lglm)
