#include "shader.h"

#include <GL/glew.h>

#include <fmt/format.h>
using namespace fmt;

#include <sstream>
#include <fstream>
using namespace std;

Shader::Shader() {}

outcome::result<Shader, std::string> Shader::loadFromFile(std::string path, Type shaderType) {
    Shader shader;
    if (shaderType == Type::Fragment) {
        shader.handle = glCreateShader(GL_FRAGMENT_SHADER);
    } else if (shaderType == Type::Vertex) {
        shader.handle = glCreateShader(GL_VERTEX_SHADER);
    } else {
        return std::string("Unknown shader type");
    }

    ifstream in(path.c_str(), ios::in);
    if (!in) {
        return format("Failed to read shader file {}", path);
    }

    std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const char* csource = source.c_str();

    glShaderSource(shader.handle, 1, &csource, nullptr);
    glCompileShader(shader.handle);
    int params = -1;
    glGetShaderiv(shader.handle, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE) {
        int maxLength = 2048;
        int length = 0;
        char log[2048];
        glGetShaderInfoLog(shader.handle, maxLength, &length, log);
        return std::string(log, length);
    }
    return shader;
}
