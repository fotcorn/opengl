#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <GL/glew.h>

class Object {
    GLuint vertexAttributeObject;
public:
    Object(std::vector<GLfloat> points, std::vector<GLfloat> colors);
    void draw();
};

#endif
