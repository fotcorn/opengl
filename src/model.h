#ifndef MODEL_H
#define MODEL_H

#include "vertex.h"

#include <string>
#include <vector>

#include <GL/glew.h>

#include "../lib/outcome.hpp"
namespace outcome = OUTCOME_V2_NAMESPACE;

class Model {
public:
    static outcome::result<Model, std::string> loadFromFile(std::string path);
    void draw();
private:
    Model () {}
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

#endif // !MODEL_H
