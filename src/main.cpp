#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

#include "shader.h"
#include "shader_program.h"

#include <fmt/format.h>
using namespace fmt;

void errorCallback(int error, const char* message) {
    cerr << "GLFW error:" << message << endl;
}

void printShaderError(GLuint shader) {
    int maxLength = 2048;
    int length = 0;
    char log[2048];
    glGetShaderInfoLog(shader, maxLength, &length, log);
    cerr << "Shader info: " << log << endl;
}

void printProgramError(GLuint shader) {
    int maxLength = 2048;
    int length = 0;
    char log[2048];
    glGetProgramInfoLog(shader, maxLength, &length, log);
    cerr << "Program info: " << log << endl;
}

outcome::result<ShaderProgram, std::string> loadShaders() {
    auto fragmentShader = Shader::loadFromFile("shaders/fragment.glsl", Shader::Type::Fragment);
    if (!fragmentShader) {
        return format("Failed to load fragment shader: {}", fragmentShader.error());
    }
    auto vertexShader = Shader::loadFromFile("shaders/vertex.glsl", Shader::Type::Vertex);
    if (!vertexShader) {
        return format("Failed to load vertex shader: {}", vertexShader.error());
    }

    ShaderProgram program;
    program.attachShader(vertexShader.value());
    program.attachShader(fragmentShader.value());
    program.setAttribLocation("vertex_position", 0);
    program.setAttribLocation("vertex_color", 1);

    auto linkOK = program.link();
    if (!linkOK) {
        return linkOK.error();
    }
    return program;
}


int main() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW3" << endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer

    GLuint vertexAttributeObject = 0;
    glGenVertexArrays(1, &vertexAttributeObject); // one attribute
    glBindVertexArray(vertexAttributeObject);

    // vertex positions
    GLfloat points[] = {
         0.0f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };
    GLuint vertexPositions = 0;
    glGenBuffers(1, &vertexPositions);  // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositions); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // vertex colors
    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    GLuint vertexColors = 0;
    glGenBuffers(1, &vertexColors);  // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexColors); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(1); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexColors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    auto shaderProgramResult = loadShaders();
    if (!shaderProgramResult) {
        cerr << shaderProgramResult.error();
        return 1;
    }
    ShaderProgram program = shaderProgramResult.value();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.use();
        glBindVertexArray(vertexAttributeObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
