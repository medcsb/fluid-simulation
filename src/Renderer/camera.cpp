#include "camera.hpp"

void Camera::init(float width, float height) {
    updateProjectionMatrix(width, height);
    updateViewMatrix();
}

void Camera::updateProjectionMatrix(float width, float height) {
    projectionMatrix = glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
}

void Camera::updateOrthographicMatrix(float near, float far) {
    projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near, far);
}

void Camera::updateViewMatrix() {
    // Update the front vector based on yaw and pitch
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    // update up vector based on roll
    up = {0.0f, 1.0f, 0.0f};
    glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front);
    up = glm::normalize(glm::vec3(rollMatrix * glm::vec4(up, 0.0f)));

    // update right vector
    right = glm::normalize(glm::cross(front, up));

    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::processRotation(float deltaYaw, float deltaPitch, float deltaRoll) {
    yaw += deltaYaw;
    pitch += deltaPitch;
    roll += deltaRoll;

    // Clamp pitch to avoid gimbal lock
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateViewMatrix();
}

void Camera::move(const glm::vec3& offset) {
    position += offset;
    updateViewMatrix();
}