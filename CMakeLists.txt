project(opengl)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# make clang and g++ pedantic with warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER MATCHES "clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
endif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER MATCHES "clang")

# make conan library includes SYSTEM imports, this way clang/g++ ignores warning from these header files
set(CONAN_SYSTEM_INCLUDES ON)

# use headers and link libraries downloaded by conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(opengl src/program.cpp src/main.cpp src/shader.cpp src/shader_program.cpp src/object.cpp src/model.cpp src/texture.cpp
                      src/gui/imgui_impl_opengl3.cpp src/gui/imgui_impl_glfw.cpp)
target_link_libraries(opengl ${CONAN_LIBS})
