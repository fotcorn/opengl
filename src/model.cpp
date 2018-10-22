#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
using namespace std;

int main() {

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("./model/Corvette-F3.obj", aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return 1;
    }

    cout << scene->mMeshes[0]->mNumVertices << endl;

    auto mesh = scene->mMeshes[0];

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
    }
}