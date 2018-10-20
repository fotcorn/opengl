project(opengl)
cmake_minimum_required(VERSION 2.8.12)
add_definitions("-std=c++14")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(lib/fmt/include)

add_executable(opengl src/main.cpp src/shader.cpp src/shader_program.cpp src/object.cpp lib/fmt/src/format.cc)
target_link_libraries(opengl ${CONAN_LIBS})
