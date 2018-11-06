#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "model.h"
#include "object.h"
#include "shader_program.h"
#include "texture.h"

#include <memory>

class Program {
public:
    void init();
    void mainLoop();

private:
    // methods
    void initGlfw();
    void initGlew();
    void initOpenGL();
    void initGui();
    void loadModel();
    void initLight();
    void initHeightMap();
    void initCamera();

    void handleInput();

    void mouseCursorPositionCallback(double xPosition, double yPosition);
    void mouseScrollCallback(double xOffset, double yOffset);

    // members
    GLFWwindow* window = nullptr;

    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    // spaceship
    std::shared_ptr<ShaderProgram> spaceShipShaderProgram;
    std::shared_ptr<Model> spaceShip;
    glm::mat4 spaceShipModelMatrix = glm::mat4(1.0f);

    // light
    std::shared_ptr<ShaderProgram> lightShaderProgram;
    std::shared_ptr<Object> light;

    // heightmap
    std::shared_ptr<ShaderProgram> heightMapShaderProgram;
    std::shared_ptr<Object> heightMap;

    bool drawGui = false;

    // movement
    float speed = 0;

    // camera
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    float cameraDistance = 5.0f;
    bool firstMouse = true;
    float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing
                        // to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
};

#endif // !PROGRAM_H
