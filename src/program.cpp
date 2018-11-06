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

#include "heightmap.h"
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
    this->initHeightMap();
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
    glfwSetWindowUserPointer(this->window, (void*)this);
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
    glfwSetCursorPosCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        Program* program = (Program*)glfwGetWindowUserPointer(window);
        program->mouseCursorPositionCallback(xPosition, yPosition);
    });
    glfwSetScrollCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        Program* program = (Program*)glfwGetWindowUserPointer(window);
        program->mouseScrollCallback(xPosition, yPosition);
    });
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void Program::loadModel() {
    // load model
    this->spaceShip = std::make_shared<Model>(Model::loadFromFile("assets/spaceship/Corvette-F3.obj"));
    this->spaceShip->addTexture(Texture::loadFromFile("assets/spaceship/SF_Corvette-F3_diffuse.jpg"));

    // load shader
    Shader fragmentShader = Shader::loadFromFile("shaders/model_fragment.glsl", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("shaders/model_vertex.glsl", Shader::Type::Vertex);
    this->spaceShipShaderProgram = std::make_shared<ShaderProgram>();
    this->spaceShipShaderProgram->attachShader(vertexShader);
    this->spaceShipShaderProgram->attachShader(fragmentShader);
    this->spaceShipShaderProgram->setAttribLocation("vertex_position", 0);
    this->spaceShipShaderProgram->setAttribLocation("texture_coordinate", 1);
    this->spaceShipShaderProgram->link();

    this->spaceShipRotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
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

void Program::initHeightMap() {
    this->heightMap = std::make_shared<Object>(loadHeightMap("assets/heightmap.png"));
    Shader fragmentShader = Shader::loadFromFile("shaders/heightmap_fragment.glsl", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("shaders/heightmap_vertex.glsl", Shader::Type::Vertex);
    this->heightMapShaderProgram = std::make_shared<ShaderProgram>();
    this->heightMapShaderProgram->attachShader(vertexShader);
    this->heightMapShaderProgram->attachShader(fragmentShader);
    this->heightMapShaderProgram->setAttribLocation("vertex_position", 0);
    this->heightMapShaderProgram->link();
}

void Program::initCamera() {
    this->projectionMatrix = glm::perspective(glm::radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);
}

void Program::mainLoop() {
    bool wireframe = false;

    glm::vec3 cameraPosition = glm::vec3(3.0, 5.0, -4.0);
    glm::vec3 lightPosition = glm::vec3(-15.0, 15.0, 5.0);

    glm::vec3 heightMapPosition = glm::vec3(-100.0, -15.0, -100.0);
    glm::vec3 heightMapScale = glm::vec3(1.0f, 2.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        this->handleInput();

        glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 directionVector = this->spaceShipRotation * forward;
        this->spaceShipPosition += directionVector * (speed * deltaTime * 10.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // world space to camera space
        glm::mat4 view =
            glm::lookAt(this->cameraFront * this->cameraDistance, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        // draw space ship
        glm::mat4 spaceShipModelMatrix = glm::translate(glm::mat4(1.0f), this->spaceShipPosition);
        spaceShipModelMatrix = glm::scale(spaceShipModelMatrix, glm::vec3(0.001, 0.001, 0.001));
        spaceShipModelMatrix *= glm::toMat4(this->spaceShipRotation);

        glm::mat4 mvp = this->projectionMatrix * view * spaceShipModelMatrix;
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
        this->light->draw(wireframe);

        // draw heightmap
        glm::mat4 heightMapModel = glm::mat4(1.0f);
        heightMapModel = glm::scale(heightMapModel, heightMapScale);
        heightMapModel = glm::translate(heightMapModel, heightMapPosition);
        mvp = this->projectionMatrix * view * heightMapModel;
        this->heightMapShaderProgram->use();
        this->heightMapShaderProgram->setUniform("mvp", mvp);
        this->heightMap->draw(wireframe);

        if (drawGui) {
            // draw gui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat("Camera X", &cameraPosition.x, -10.0f, 10.0f);
            ImGui::SliderFloat("Camera Y", &cameraPosition.y, -10.0f, 10.0f);
            ImGui::SliderFloat("Camera Z", &cameraPosition.z, -10.0f, 10.0f);

            ImGui::SliderFloat("Light X", &lightPosition.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Light Y", &lightPosition.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Light Z", &lightPosition.z, -100.0f, 100.0f);

            ImGui::SliderFloat("Heightmap X", &heightMapPosition.x, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Heightmap Y", &heightMapPosition.y, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Heightmap Z", &heightMapPosition.z, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Heightmap Scale X", &heightMapScale.x, -5.0f, 5.0f);
            ImGui::SliderFloat("Heightmap Scale Y", &heightMapScale.y, -5.0f, 5.0f);
            ImGui::SliderFloat("Heightmap Scale Z", &heightMapScale.z, -5.0f, 5.0f);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void Program::handleInput() {
    static bool ctrlDown = false;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        this->speed += 1.0 * deltaTime;
        if (this->speed > 100.0) {
            this->speed = 100.0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        this->speed -= 1.0 * deltaTime;
        if (this->speed < 0.0) {
            this->speed = 0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        this->spaceShipRotation =
            glm::rotate(this->spaceShipRotation, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        this->spaceShipRotation =
            glm::rotate(this->spaceShipRotation, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->spaceShipRotation =
            glm::rotate(this->spaceShipRotation, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->spaceShipRotation =
            glm::rotate(this->spaceShipRotation, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        this->spaceShipRotation =
            glm::rotate(this->spaceShipRotation, glm::radians(40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->spaceShipRotation =
            glm::rotate(this->spaceShipRotation, glm::radians(-40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        ctrlDown = true;
    } else if (ctrlDown) {
        ctrlDown = false;
        this->drawGui = !drawGui;
        if (this->drawGui) {
            glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            double mouseX, mouseY;
            glfwGetCursorPos(this->window, &mouseX, &mouseY);
            this->lastX = mouseX;
            this->lastY = mouseY;
        }
    }
}

void Program::mouseCursorPositionCallback(double xPosition, double yPosition) {
    if (this->drawGui) {
        return;
    }
    if (firstMouse) {
        lastX = xPosition;
        lastY = yPosition;
        firstMouse = false;
    }

    float xoffset = xPosition - lastX;
    float yoffset = yPosition - lastY; // reversed since y-coordinates go from bottom to top
    lastX = xPosition;
    lastY = yPosition;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void Program::mouseScrollCallback(double xOffset, double yOffset) {
    if (this->drawGui) {
        ImGui_ImplGlfw_ScrollCallback(this->window, xOffset, yOffset);
    } else {
        cameraDistance -= yOffset;
        if (cameraDistance < 2) {
            cameraDistance = 1;
        }
    }
}
