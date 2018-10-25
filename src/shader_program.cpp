#include "shader_program.h"

#include <string>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "../lib/outcome.hpp"
namespace outcome = OUTCOME_V2_NAMESPACE;

ShaderProgram::ShaderProgram() {
    this->handle = glCreateProgram();
}

void ShaderProgram::attachShader(Shader shader) {
    glAttachShader(this->handle, shader.handle);
}

void ShaderProgram::setAttribLocation(std::string attribute, unsigned int location)  {
    glBindAttribLocation(this->handle, location, attribute.c_str());
}

outcome::result<void, std::string> ShaderProgram::link() {
    glLinkProgram(this->handle);
    int params = -1;
    glGetProgramiv(this->handle, GL_LINK_STATUS, &params);
    if (params != GL_TRUE) {
        GLint logLength;
        glGetProgramiv(this->handle, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> error(logLength);
        glGetProgramInfoLog(this->handle, logLength, nullptr, &error[0]);
        return std::string(error.begin(), error.end());
    }
    return outcome::success();
}

void ShaderProgram::use() {
     glUseProgram(this->handle);
}

void ShaderProgram::setUniform(std::string uniform, glm::mat4 data) {
    int location = glGetUniformLocation(this->handle, uniform.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data));
}
