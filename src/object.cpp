#include "object.h"

Object::Object(std::vector<GLfloat> points, std::vector<GLfloat> colors) {
    glGenVertexArrays(1, &this->vertexAttributeObject); // one attribute
    glBindVertexArray(this->vertexAttributeObject);

    GLuint vertexPositions = 0;
    glGenBuffers(1, &vertexPositions); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositions); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * points.size(), &points[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint vertexColors = 0;
    glGenBuffers(1, &vertexColors); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexColors); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), &colors[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(1); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexColors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void Object::draw() {
    glBindVertexArray(this->vertexAttributeObject);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
