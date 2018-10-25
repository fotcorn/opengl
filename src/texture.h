#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include <string>

#include <outcome/outcome.hpp>
namespace outcome = OUTCOME_V2_NAMESPACE;

class Texture {
public:
    static outcome::result<Texture, std::string> loadFromFile(std::string path);

    void bind();

private:
    GLuint handle;
};

#endif // !TEXTURE_H
