#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>
using namespace fmt;

#include "shader.h"
#include "shader_program.h"
#include "object.h"

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

void errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,const GLchar* message, const void* userParam) {
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
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

    GLFWwindow* window = glfwCreateWindow(1024, 800, "Hello World", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorCallback, 0);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer

    Object object1({
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    }, {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    });



    // model to world space
    glm::mat4 model;
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // world space to camera space
    glm::mat4 view;
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // camera space to projection/2D space
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);

    glm::mat4 mvp = projection * view * model;

    auto shaderProgramResult = loadShaders();
    if (!shaderProgramResult) {
        cerr << shaderProgramResult.error();
        return 1;
    }
    ShaderProgram program = shaderProgramResult.value();
    program.setUniform("mvp", projection);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.use();
        object1.draw();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
