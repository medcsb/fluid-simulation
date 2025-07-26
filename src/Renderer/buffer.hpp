#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <glad/glad.h>
#include <vector>

struct AttributeInfo {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized = GL_FALSE;
    GLsizei stride;
    size_t offset;
};

struct VAOConfigInfo {
    std::vector<AttributeInfo> attributes;
    size_t sizeOfVertex;
    size_t sizeOfVertexData;
    size_t verticesCount;
    size_t indexCount = 0;
    bool useEBO = false;
    GLenum drawMode = GL_TRIANGLES;
};

class Buffer {
public:
    GLuint VBO = 0, VAO = 0, EBO = 0;
    GLenum drawMode = GL_TRIANGLES;
    uint32_t indexCount = 0;
    bool hasEBO = false;
    bool isInitialized = false;

    Buffer() = default;
    ~Buffer() {};

    void init(const void* vertexData, const uint32_t* indexData, const VAOConfigInfo& config);
    void unbindVAO() const {glBindVertexArray(0);}
    void bind() const;
    void draw() const;
    void cleanup();

};

#endif // BUFFER_HPP