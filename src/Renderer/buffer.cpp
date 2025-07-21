#include "buffer.hpp"

#include "log_utils.hpp"


void Buffer::init(const void* vertexData, const uint32_t* indexData, const VAOConfigInfo& config) {
    if (isInitialized) {
        warn("Buffer is already initialized. Cleaning up before re-initializing, might not be intended behavior.");
        cleanup();
    }
    isInitialized = true;
    hasEBO = config.useEBO;
    drawMode = config.drawMode;
    indexCount = config.indexCount;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if (config.useEBO) glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, config.sizeOfVertexData, vertexData, GL_STATIC_DRAW);

    if (config.useEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, config.indexCount * sizeof(uint32_t), indexData, GL_STATIC_DRAW);
    }

    for (const auto& attr : config.attributes) {
        glVertexAttribPointer(
            attr.index,
            attr.size,
            attr.type,
            attr.normalized,
            attr.stride,
            reinterpret_cast<void*>(attr.offset)
        );
        glEnableVertexAttribArray(attr.index);
    }
}

void Buffer::bind() const {
    if (!isInitialized) {
        error("Buffer is not initialized. WTF are you doing? You cant bind it");
    }
    glBindVertexArray(VAO);
}

void Buffer::draw() const {
    if (!isInitialized) {
        error("Buffer is not initialized. Are you DUMB? You cant draw it");
    }
    if (hasEBO) glDrawElements(drawMode, indexCount, GL_UNSIGNED_INT, 0);
    else glDrawArrays(drawMode, 0, indexCount);
}

void Buffer::cleanup() {
    if (!isInitialized) {
        softWarn("Buffer is not initialized. Nothing to clean up.");
        return;
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (hasEBO) glDeleteBuffers(1, &EBO);
    isInitialized = false;
    hasEBO = false;
}
