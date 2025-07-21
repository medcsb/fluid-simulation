#include "model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <stdexcept>

Model::Model() {}
Model::~Model() {}

void Model::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Model::loadTexture(const std::string& path) {
    glGenTextures(1, &texture);
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
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}
    };
    indices = { 2, 1, 0 };
}

void Model::simpleQuad() {
    vertices = {
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };
    indices = {0, 1, 3,
               1, 2, 3};
}

void Model::simpleCube() {
    verticesNoTex = {
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    indices = {
        0, 1, 2, 1, 3, 2,
        4, 5, 6, 5, 7, 6,
        0, 1, 4, 1, 5, 4,
        2, 3, 6, 3, 7, 6,
        0, 2, 4, 2, 6, 4,
        1, 3, 5, 3, 7, 5
    };
}

void Model::texturedCube() {
    vertices = {
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

        // Front face
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

        // Left face
        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}},

        // Right face
        {{0.5f, 0.5f, 0.5f}, {0.2f, 0.8f, 0.5f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.2f, 0.8f, 0.5f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.2f, 0.8f, 0.5f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.2f, 0.8f, 0.5f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.2f, 0.8f, 0.5f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.2f, 0.8f, 0.5f}, {1.0f, 0.0f}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.7f, 0.7f, 0.7f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.7f, 0.7f, 0.7f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.7f, 0.7f, 0.7f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.7f, 0.7f, 0.7f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.7f, 0.7f, 0.7f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.7f, 0.7f, 0.7f}, {0.0f, 1.0f}},

        // Top face
        {{-0.5f, 0.5f, -0.5f}, {0.9f, 0.4f, 0.3f}, {0.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.9f, 0.4f, 0.3f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.9f, 0.4f, 0.3f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.9f, 0.4f, 0.3f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.9f, 0.4f, 0.3f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.9f, 0.4f, 0.3f}, {0.0f, 1.0f}},
    };

    indices = {
        0, 1, 2, 3, 4, 5,       // Back face
        6, 7, 8, 9, 10, 11,     // Front face
        12, 13, 14, 15, 16, 17, // Left face
        18, 19, 20, 21, 22, 23, // Right face
        24, 25, 26, 27, 28, 29, // Bottom face
        30, 31, 32, 33, 34, 35  // Top face
    };
}
