#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "shader.h"
#include <string>

#include <GL/glew.h>

#include <glm/mat4x4.hpp>

#include <outcome/outcome.hpp>
namespace outcome = OUTCOME_V2_NAMESPACE;

class ShaderProgram {
public:
    ShaderProgram();
    void attachShader(Shader shader);
    void setAttribLocation(std::string attribute, unsigned int location);
    void setUniform(std::string uniform, glm::mat4 data);
    outcome::result<void, std::string> link();
    void use();

private:
    GLuint handle;
};

#endif
