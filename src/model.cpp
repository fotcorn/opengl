#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fmt/format.h>
using namespace fmt;

outcome::result<Model, std::string> Model::loadFromFile(std::string path) {
    Model model;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return format("Failed to load model from {}: {}", path, importer.GetErrorString());
    }

    // load vertex positions
    auto mesh = scene->mMeshes[0];
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        model.vertices.push_back(vertex);
    }

    // load vertex incides
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            model.indices.push_back(face.mIndices[j]);
        }
    }  

    glGenVertexArrays(1, &model.vao); // one attribute
    glBindVertexArray(model.vao);

    // load vertex positions
    glGenBuffers(1, &model.vbo);  // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model.vertices.size(), &model.vertices[0], GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // load vertex incides
    glGenBuffers(1, &model.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);

    return model;
}

void Model::draw() {
    // glBindVertexArray(this->model.vao);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
}