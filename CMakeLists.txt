project(opengl)
cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(opengl src/main.cpp src/shader.cpp src/shader_program.cpp src/object.cpp src/model.cpp src/texture.cpp)
target_link_libraries(opengl ${CONAN_LIBS})
