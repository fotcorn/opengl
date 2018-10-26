#include "program.h"

#include <iostream>
#include <stdexcept>

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

static void glfwErrorCallback(int /*unused*/, const char* message) {
    std::cerr << "GLFW error:" << message << std::endl;
}

static void glfwFramebufferSizeCallback(GLFWwindow* /*unused*/, int width, int height) {
    glViewport(0, 0, width, height);
}

static void openglErrorCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/, GLenum severity, GLsizei /*unused*/,
                                const GLchar* message, const void* /*unused*/) {
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 800;

void Program::init() {
    this->initGlfw();
    this->initGlew();
    this->initOpenGL();
    this->initGui();
    this->loadModel();
    this->initLight();
    this->initCamera();
}

void Program::initGlfw() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW3");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    this->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    if (!this->window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, glfwFramebufferSizeCallback);
}

void Program::initGlew() {
    glewExperimental = GL_TRUE;
    glewInit();
}

void Program::initOpenGL() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openglErrorCallback, nullptr);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Program::initGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, false);
    glfwSetMouseButtonCallback(this->window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetScrollCallback(this->window, ImGui_ImplGlfw_ScrollCallback);

    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void Program::loadModel() {
    // load model
    this->spaceShip = std::make_shared<Model>(Model::loadFromFile("model/Corvette-F3.obj"));
    this->spaceShip->addTexture(Texture::loadFromFile("model/SF_Corvette-F3_diffuse.jpg"));

    // load shader
    Shader fragmentShader = Shader::loadFromFile("shaders/model_fragment.glsl", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("shaders/model_vertex.glsl", Shader::Type::Vertex);
    this->spaceShipShaderProgram = std::make_shared<ShaderProgram>();
    this->spaceShipShaderProgram->attachShader(vertexShader);
    this->spaceShipShaderProgram->attachShader(fragmentShader);
    this->spaceShipShaderProgram->setAttribLocation("vertex_position", 0);
    this->spaceShipShaderProgram->setAttribLocation("texture_coordinate", 1);
    this->spaceShipShaderProgram->link();

    // model to world space
    this->spaceShipModelMatrix = glm::scale(this->spaceShipModelMatrix, glm::vec3(0.001, 0.001, 0.001));
}

void Program::initLight() {
    this->light = std::make_shared<Object>(Object(
        {
            // vertices
            // front
            glm::vec3(-1.0, -1.0, 1.0),
            glm::vec3(1.0, -1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(-1.0, 1.0, 1.0),
            // back
            glm::vec3(-1.0, -1.0, -1.0),
            glm::vec3(1.0, -1.0, -1.0),
            glm::vec3(1.0, 1.0, -1.0),
            glm::vec3(-1.0, 1.0, -1.0),
        },
        {
            // colors
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
        },
        {
            // indices
            // front
            glm::vec3(0, 1, 2),
            glm::vec3(2, 3, 0),
            // right
            glm::vec3(1, 5, 6),
            glm::vec3(6, 2, 1),
            // back
            glm::vec3(7, 6, 5),
            glm::vec3(5, 4, 7),
            // left
            glm::vec3(4, 0, 3),
            glm::vec3(3, 7, 4),
            // bottom
            glm::vec3(4, 5, 1),
            glm::vec3(1, 0, 4),
            // top
            glm::vec3(3, 2, 6),
            glm::vec3(6, 7, 3),
        }));
    Shader fragmentShader = Shader::loadFromFile("shaders/light_fragment.glsl", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("shaders/light_vertex.glsl", Shader::Type::Vertex);
    this->lightShaderProgram = std::make_shared<ShaderProgram>();
    this->lightShaderProgram->attachShader(vertexShader);
    this->lightShaderProgram->attachShader(fragmentShader);
    this->lightShaderProgram->setAttribLocation("vertex_position", 0);
    // this->lightShaderProgram->setAttribLocation("vertex_color", 1);
    this->lightShaderProgram->link();
}

void Program::initCamera() {
    this->projectionMatrix = glm::perspective(glm::radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);
}

void Program::mainLoop() {
    bool wireframe = false;

    float cameraX = 3.0f;
    float cameraY = 5.0f;
    float cameraZ = -4.0f;

    glm::vec3 lightPosition = glm::vec3(-15.0, 15.0, 5.0);

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // handle input
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            this->spaceShipModelMatrix =
                glm::rotate(this->spaceShipModelMatrix, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            this->spaceShipModelMatrix =
                glm::rotate(this->spaceShipModelMatrix, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            this->spaceShipModelMatrix =
                glm::rotate(this->spaceShipModelMatrix, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            this->spaceShipModelMatrix =
                glm::rotate(this->spaceShipModelMatrix, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            this->spaceShipModelMatrix =
                glm::rotate(this->spaceShipModelMatrix, glm::radians(40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            this->spaceShipModelMatrix =
                glm::rotate(this->spaceShipModelMatrix, glm::radians(-40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // world space to camera space
        glm::mat4 view;
        view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        // draw space ship
        glm::mat4 mvp = this->projectionMatrix * view * this->spaceShipModelMatrix;
        this->spaceShipShaderProgram->use();
        this->spaceShipShaderProgram->setUniform("mvp", mvp);
        this->spaceShip->draw(wireframe);

        // draw light
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::scale(lightModel, glm::vec3(0.1, 0.1, 0.1));
        lightModel = glm::translate(lightModel, lightPosition);
        mvp = this->projectionMatrix * view * lightModel;
        this->lightShaderProgram->use();
        this->lightShaderProgram->setUniform("mvp", mvp);
        this->light->draw();

        // draw gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::SliderFloat("Camera X", &cameraX, -10.0f, 10.0f);
        ImGui::SliderFloat("Camera Y", &cameraY, -10.0f, 10.0f);
        ImGui::SliderFloat("Camera Z", &cameraZ, -10.0f, 10.0f);

        ImGui::SliderFloat("Light X", &lightPosition.x, -100.0f, 100.0f);
        ImGui::SliderFloat("Light Y", &lightPosition.y, -100.0f, 100.0f);
        ImGui::SliderFloat("Light Z", &lightPosition.z, -100.0f, 100.0f);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
