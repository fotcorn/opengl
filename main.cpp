#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

int main() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW3" << endl;
        return 1;
    }

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

    // TODO

    glfwTerminate();
    return 0;
}
