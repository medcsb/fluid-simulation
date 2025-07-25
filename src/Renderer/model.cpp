#include "model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <iostream>
#include <stdexcept>

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hashCombine(seed, rest), ...);
};

namespace std {
    template <> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            size_t seed = 0;
            hashCombine(seed, vertex.pos, vertex.normal, vertex.texCoord);
            return seed;
        }
    };
}

Model::Model() {}
Model::~Model() {}

void Model::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Model::setTextureParams(GLenum wrapS, GLenum wrapT, GLenum minFilter, GLenum magFilter) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Model::loadTexture(const std::string& path) {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
}

void Model::simpleTriangle() {
    vertices = {
        {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}
    };
    indices = { 2, 1, 0 };
}

void Model::simpleQuad() {
    vertices = {
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}
    };
    indices = {0, 1, 3,
               1, 2, 3};
}

void Model::simpleCube() {
    verticesNoTex = {
        // Front face (normal: 0.0f, 0.0f, -1.0f)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, // 0
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  // 1
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  // 2
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},   // 3

        // Back face (normal: 0.0f, 0.0f, 1.0f)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},   // 4
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    // 5
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    // 6
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},     // 7

        // Right face (normal: 1.0f, 0.0f, 0.0f)
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},  // (re-using vertex 1)
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},   // (re-using vertex 3)
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},    // (re-using vertex 5)
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},     // (re-using vertex 7)

        // Left face (normal: -1.0f, 0.0f, 0.0f)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, // (re-using vertex 0)
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},  // (re-using vertex 2)
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},  // (re-using vertex 4)
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},   // (re-using vertex 6)

        // Top face (normal: 0.0f, 1.0f, 0.0f)
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   // (re-using vertex 2)
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},    // (re-using vertex 3)
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},    // (re-using vertex 6)
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},     // (re-using vertex 7)

        // Bottom face (normal: 0.0f, -1.0f, 0.0f)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},// (re-using vertex 0)
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, // (re-using vertex 1)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}}, // (re-using vertex 4)
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}}   // (re-using vertex 5)
    };

    indices = {
        // Front face
        2, 1, 0, 2, 3, 1,
        // Back face
        4, 5, 6, 5, 7, 6,
        // Right face
        8, 9, 10, 9, 11, 10,
        // Left face
        14, 13, 12, 14, 15, 13,
        // Top face
        18, 17, 16, 18, 19, 17,
        // Bottom face
        20, 21, 22, 21, 23, 22
    };
}

void Model::texturedCube() {
    vertices = {
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

        // Front face
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

        // Left face
        {{-0.5f, 0.5f, 0.5f}, {-1.0, 0.0, 0.0}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {-1.0, 0.0, 0.0}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0, 0.0, 0.0}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0, 0.0, 0.0}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {-1.0, 0.0, 0.0}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {-1.0, 0.0, 0.0}, {1.0f, 0.0f}},

        // Right face
        {{0.5f, 0.5f, 0.5f}, {1.0, 0.0, 0.0}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0, 0.0, 0.0}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0, 0.0, 0.0}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0, 0.0, 0.0}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0, 0.0, 0.0}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0, 0.0, 0.0}, {1.0f, 0.0f}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

        // Top face
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };

    indices = {
        2, 1, 0, 5, 4, 3,       // Back face
        6, 7, 8, 9, 10, 11,     // Front face
        12, 13, 14, 15, 16, 17, // Left face
        20, 19, 18, 23, 22, 21, // Right face
        24, 25, 26, 27, 28, 29, // Bottom face
        32, 31, 30, 35, 34, 33  // Top face
    };
}

void Model::sphereNoTex() {

}

void Model::loadModel(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error("TinyObj failed to load objects ==> " + warn + err);
    }
    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }
            // TODO: have a better hashing for unique values when loading .obj files
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}
