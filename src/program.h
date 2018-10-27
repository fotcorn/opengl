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
    void initCamera();

    // members
    GLFWwindow* window = nullptr;
    std::shared_ptr<ShaderProgram> spaceShipShaderProgram;
    std::shared_ptr<Model> spaceShip;

    std::shared_ptr<ShaderProgram> lightShaderProgram;
    std::shared_ptr<Object> light;

    glm::mat4 spaceShipModelMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    bool drawGui = false;
};

#endif // !PROGRAM_H
