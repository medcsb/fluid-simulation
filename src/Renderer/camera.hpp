#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    glm::mat4 viewMatrix{1.0f};
    glm::mat4 projectionMatrix{1.0f};
    glm::vec3 position{0.0f, 0.0f, 3.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 right{1.0f, 0.0f, 0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float yaw{-90.0f};
    float pitch{0.0f};
    float roll{0.0f};
    float fov{45.0f};

    Camera() {}
    ~Camera() {}

    void init(float width, float height);
    void updateProjectionMatrix(float width, float height);
    void updateOrthographicMatrix(float near, float far);
    void updateViewMatrix();
    void processRotation(float deltaYaw, float deltaPitch, float deltaRoll);
    void move(const glm::vec3& offset);
    glm::mat4& getViewMatrix() {return viewMatrix;}
    glm::mat4& getProjectionMatrix() {return projectionMatrix;}
    glm::vec3& getPosition() {return position;}
    glm::vec3& getFront() {return front;}
    glm::vec3& getRight() {return right;}
    glm::vec3& getUp() {return up;}
    float& getYaw() {return yaw;}
    float& getPitch() {return pitch;}
    float& getRoll() {return roll;}
    float& getFov() {return fov;}

    void setPosition(const glm::vec3& pos) {position = pos;}
    void setFront(const glm::vec3& f) {front = f;}
    void setRight(const glm::vec3& r) {right = r;}
    void setUp(const glm::vec3& u) {up = u;}
    void setYaw(float y) {yaw = y;}
    void setPitch(float p) {pitch = p;}
    void setRoll(float r) {roll = r;}
    void setFov(float f) {fov = f;}

};

#endif // CAMERA_HPP