#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <fmt/format.h>
using namespace fmt;

#include <imgui.h>

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"

#include "model.h"
#include "shader.h"
#include "shader_program.h"

void glfwErrorCallback(int /*unused*/, const char* message) {
    cerr << "GLFW error:" << message << endl;
}

void glfwFramebufferSizeCallback(GLFWwindow* /*unused*/, int width, int height) {
    glViewport(0, 0, width, height);
}

void openglErrorCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/, GLenum severity, GLsizei /*unused*/,
                         const GLchar* message, const void* /*unused*/) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
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
    program.setAttribLocation("texture_coordinate", 1);

    auto linkOK = program.link();
    if (!linkOK) {
        return linkOK.error();
    }
    return program;
}

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 800;

int main() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW3" << endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);

    // glew
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openglErrorCallback, nullptr);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    auto modelLoadResult = Model::loadFromFile("model/Corvette-F3.obj");
    if (!modelLoadResult) {
        cerr << modelLoadResult.error();
        return 1;
    }
    Model spaceShip = modelLoadResult.value();

    auto textureLoadResult = Texture::loadFromFile("model/SF_Corvette-F3_diffuse.jpg");
    if (!textureLoadResult) {
        cerr << textureLoadResult.error();
        return 1;
    }
    auto texture = textureLoadResult.value();

    spaceShip.addTexture(texture);

    // model to world space
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.001, 0.001, 0.001));

    // camera space to projection/2D space
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);

    auto shaderProgramResult = loadShaders();
    if (!shaderProgramResult) {
        cerr << shaderProgramResult.error();
        return 1;
    }
    ShaderProgram program = shaderProgramResult.value();

    bool wireframe = false;

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);

    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    float cameraX = 3.0f;
    float cameraY = 5.0f;
    float cameraZ = -4.0f;

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // handle input
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // float cameraSpeed = 0.05f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            model = glm::rotate(model, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            model = glm::rotate(model, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            model = glm::rotate(model, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            model = glm::rotate(model, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            model = glm::rotate(model, glm::radians(40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            model = glm::rotate(model, glm::radians(-40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        // world space to camera space
        glm::mat4 view;
        view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        // full camera matrix
        glm::mat4 mvp = projection * view * model;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.use();
        program.setUniform("mvp", mvp);
        spaceShip.draw(wireframe);

        // draw gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::SliderFloat("Camera X", &cameraX, -10.0f, 10.0f);
        ImGui::SliderFloat("Camera Y", &cameraY, -10.0f, 10.0f);
        ImGui::SliderFloat("Camera Z", &cameraZ, -10.0f, 10.0f);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
