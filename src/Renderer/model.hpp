#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "stb_image.h"

#include <vector>
#include <string>

class Model {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    };
private:
    uint32_t texture;
public:

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Model();
    ~Model();

    void bindTexture();

    void simpleTriangle();
    void simpleQuad();
    void loadTexture(const std::string& path);

    std::vector<Vertex>& getVertices() {return vertices;}
    std::vector<uint32_t>& getIndices() {return indices;}
};

#endif // MODEL_HPP