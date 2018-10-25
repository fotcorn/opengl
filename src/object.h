#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>
#include <vector>

class Object {
    GLuint vertexAttributeObject = 0;

public:
    Object(std::vector<GLfloat> points, std::vector<GLfloat> colors);
    void draw();
};

#endif
