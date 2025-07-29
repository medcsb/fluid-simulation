#include "buffer.hpp"

#include "log_utils.hpp"

void Buffer::initInstanced(const void* vertexData, const void* instanceData, const uint32_t* indexData, InstanceConfigInfo& config) {
    if (isInitialized) {
        warn("Buffer is already initialized. Cleaning up before re-initializing, might not be intended behavior.");
        cleanup();
    }

    isInitialized = true;
    hasEBO = config.useEBO;
    drawMode = config.drawMode;
    indexCount = config.indexCount;
    instancesCount = config.instancesCount;
    this->drawMode = drawMode;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);
    if (hasEBO) glGenBuffers(1, &EBO);
    glGenBuffers(1, &SSBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, config.sizeOfVertexData, vertexData, GL_STATIC_DRAW);

    // setup static attributes
    for (const auto& attr : config.staticAttribs) {
        glVertexAttribPointer(
            attr.index,
            attr.size,
            attr.type,
            attr.normalized,
            attr.stride,
            reinterpret_cast<void*>(attr.offset));
        glEnableVertexAttribArray(attr.index);
    }

    if (hasEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, config.indexCount * sizeof(uint32_t), indexData, GL_STATIC_DRAW);
    }

    // setup instance attributes
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instancesCount * config.sizeOfInstance, nullptr, GL_DYNAMIC_DRAW);

    for (const auto& attr : config.instanceAttribs) {
        glVertexAttribPointer(
            attr.index,
            attr.size,
            attr.type,
            attr.normalized,
            attr.stride,
            reinterpret_cast<void*>(attr.offset));
        glEnableVertexAttribArray(attr.index);
        glVertexAttribDivisor(attr.index, 1); // Set divisor for instanced rendering
    }

    // Setup SSBO for instance data
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, instancesCount * config.sizeOfInstance, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindVertexArray(0);
}

void Buffer::bindInstanced() const {
    if (!isInitialized) error("Buffer is not initialized. WTF are you doing? You cant bind it");
    glBindVertexArray(VAO);
}

void Buffer::drawInstanced() const {
    if (!isInitialized) error("Buffer is not initialized. Are you DUMB? You cant draw it");
    if (hasEBO) glDrawElementsInstanced(drawMode, indexCount, GL_UNSIGNED_INT, 0, instancesCount);
    else glDrawArraysInstanced(drawMode, 0, indexCount, instancesCount);
}

void Buffer::updateInstanceData(const void* instanceData, size_t instanceSize, size_t instanceCount) {
    if (!isInitialized) {
        error("Buffer is not initialized. Cannot update instance data.");
        return;
    }
    instancesCount = instanceCount;
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceSize * instanceCount, instanceData);
}

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
    if (!isInitialized) error("Buffer is not initialized. WTF are you doing? You cant bind it");
    glBindVertexArray(VAO);
}

void Buffer::draw() const {
    if (!isInitialized) error("Buffer is not initialized. Are you DUMB? You cant draw it");
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
