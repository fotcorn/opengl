#include "shader_program.h"

#include <string>

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
        int maxLength = 2048;
        int length = 0;
        char log[2048];
        glGetProgramInfoLog(this->handle, maxLength, &length, log);
        return std::string(log, length);
    }
    return outcome::success();
}

void ShaderProgram::use() {
     glUseProgram(this->handle);
}
