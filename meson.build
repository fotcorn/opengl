project('opengl', 'cpp', default_options : ['cpp_std=c++14'])

opengl = dependency('GL')
glew = dependency('glew')
glfw = dependency('glfw3')

src = ['src/main.cpp', 'src/shader.cpp', 'src/shader_program.cpp', 'lib/fmt/src/format.cc']

incdir = include_directories('lib/fmt/include')

executable('opengl', src, dependencies : [opengl, glfw, glew], include_directories : incdir)
