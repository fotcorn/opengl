project('opengl', 'cpp', default_options : ['cpp_std=c++17'])

opengl = dependency('GL')
glew = dependency('glew')
glfw = dependency('glfw3')

src = ['src/main.cpp']

executable('opengl', src, dependencies : [opengl, glfw, glew])
