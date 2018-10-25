#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>

#include "../lib/outcome.hpp"
namespace outcome = OUTCOME_V2_NAMESPACE;

class Shader {
    friend class ShaderProgram;

private:
    Shader();
    GLuint handle;

public:
    enum class Type { Vertex, Fragment };
    static outcome::result<Shader, std::string> loadFromFile(std::string path, Type shaderType);
};

#endif
