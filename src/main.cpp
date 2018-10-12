#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;



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

    const char* vertexShaderSource =
    "#version 410\n"
    "in vec3 vertex_position;\n"
    "in vec3 vertex_color;\n"
    "out vec3 color;\n"
    "void main() {\n"
    "  color = vertex_color;\n"
    "  gl_Position = vec4(vertex_position, 1.0);\n"
    "}\n";

    const char* fragmentShaderSource =
    "#version 410\n"
    "in vec3 color;"
    "out vec4 fragmentColor;\n"
    "void main() {\n"
    "  fragmentColor = vec4(color, 1.0);\n"
    "}\n";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int params = -1;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE) {
        printShaderError(vertexShader);
        cerr << "vertex shader failed to compile" << endl;
        return 1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE) {
        printShaderError(fragmentShader);
        cerr << "fragment shader failed to compile" << endl;
        return 1;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindAttribLocation(shaderProgram, 0, "vertex_position");
    glBindAttribLocation(shaderProgram, 1, "vertex_color");
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &params);
    if (params != GL_TRUE) {
        printProgramError(shaderProgram);
        cerr << "failed to link shader" << endl;
        return 1;
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vertexAttributeObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
