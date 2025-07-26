#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/glad.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "stb_image.h"

#include <vector>
#include <string>

struct Vertex {
    glm::vec3 pos;
    //glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }
};
struct VertexNoTex {
    glm::vec3 pos;
    //glm::vec3 color;
    glm::vec3 normal;
};
struct Light {
    float ambientStrength = 0.1f;
    float specularStrength = 0.5f;
    int specularPower = 32;
    float opaqueVal = 0.9;
    float attenuationFactor = 1.0f;
};
struct Transform {
    glm::vec3 translationVec = glm::vec3(0.0f);
    glm::vec3 rotationVec = glm::vec3(0.0f);  // In degrees
    glm::vec3 scaleVec = glm::vec3(1.0f);

    glm::mat4 getTransformMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), translationVec);
        glm::mat4 rotation = glm::yawPitchRoll(
            glm::radians(rotationVec.y),
            glm::radians(rotationVec.x),
            glm::radians(rotationVec.z)
        );
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleVec);
        return translation * rotation * scale;
    }
};

class Model {
private:
    uint32_t texture;

public:

    std::string name;

    std::vector<Vertex> vertices;
    std::vector<VertexNoTex> verticesNoTex;
    std::vector<uint32_t> indices;
    Transform transform{};
    Light light{};
    glm::vec3 color = glm::vec3(1.0f);

    bool isTextured = false;

    Model();
    ~Model();

    void bindTexture();
    void setTextureParams(GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR);
    void setTextureParams();

    void simpleTriangle();
    void simpleQuad();
    void simpleCube();
    void texturedCube();
    void sphereNoTex();
    void loadTexture(const std::string& path);
    void loadModel(const std::string& path);

    std::vector<Vertex>& getVertices() {return vertices;}
    std::vector<VertexNoTex>& getVerticesNoTex() {return verticesNoTex;}
    std::vector<uint32_t>& getIndices() {return indices;}
    Transform& getTransform() {return transform;}
    Light& getLight() {return light;}
    glm::vec3& getColor() {return color;}
    float& getOpaqueVal() {return light.opaqueVal;}
};

#endif // MODEL_HPP