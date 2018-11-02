#include "heightmap.h"

#include <glm/vec3.hpp>
#include <vector>

#include <stb_image.h>

#include <fmt/format.h>
using namespace fmt;

Object loadHeightMap(std::string path) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        throw std::runtime_error(format("Failed to load texture {}", path));
    }
    if (nrChannels != 1) {
        throw new std::runtime_error(format("Heightmap has more than one color: {}", path));
    }

    std::vector<glm::vec3> vertices;
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            unsigned char elevation = data[w + h * width];
            vertices.push_back(glm::vec3(w, elevation / 16, h));
        }
    }

    std::vector<glm::uvec3> indices;
    for (int w = 0; w < width - 1; w++) {
        for (int h = 0; h < height - 1; h++) {
            // upper left triangle, CCW
            indices.push_back(glm::uvec3(w, w + (h + 1) * height, w + 1));
            // lower right triangle, CCW
            indices.push_back(glm::uvec3(w + (h + 1) * height, w + 1 + (h + 1) * height, w + 1));
        }
    }

    return Object(vertices, indices);
}
