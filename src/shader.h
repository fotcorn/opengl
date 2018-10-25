#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>

#include <outcome/outcome.hpp>
namespace outcome = OUTCOME_V2_NAMESPACE;

class Shader {
    friend class ShaderProgram;

private:
    Shader() = default;
    GLuint handle = 0;

public:
    enum class Type { Vertex, Fragment };
    static outcome::result<Shader, std::string> loadFromFile(const std::string& path, Type shaderType);
};

#endif
