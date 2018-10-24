#ifndef MODEL_H
#define MODEL_H

#include "vertex.h"
#include "texture.h"

#include <string>
#include <vector>

#include <GL/glew.h>

#include "../lib/outcome.hpp"
namespace outcome = OUTCOME_V2_NAMESPACE;

class Model {
public:
    static outcome::result<Model, std::string> loadFromFile(std::string path);
    void addTexture(Texture texture);
    void draw();
private:
    Model () {}
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

#endif // !MODEL_H
