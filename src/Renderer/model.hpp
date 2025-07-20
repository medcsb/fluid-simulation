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

class Model {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
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

private:

    uint32_t texture;

public:

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Transform transform{};

    Model();
    ~Model();

    void bindTexture();
    void setTextureParams();

    void simpleTriangle();
    void simpleQuad();
    void loadTexture(const std::string& path);

    std::vector<Vertex>& getVertices() {return vertices;}
    std::vector<uint32_t>& getIndices() {return indices;}
    Transform& getTransform() {return transform;}
};

#endif // MODEL_HPP