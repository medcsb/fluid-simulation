#include "cameraController.hpp"


void CameraController::processKeyboardInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, KeyMapping.FORWARD) == GLFW_PRESS) {
        camera.move(camera.getFront() * movementSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.BACKWARD) == GLFW_PRESS) {
        camera.move(-camera.getFront() * movementSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.LEFT) == GLFW_PRESS) {
        camera.move(-camera.getRight() * movementSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.RIGHT) == GLFW_PRESS) {
        camera.move(camera.getRight() * movementSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.UP) == GLFW_PRESS) {
        camera.move(camera.getUp() * movementSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.DOWN) == GLFW_PRESS) {
        camera.move(-camera.getUp() * movementSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.ROLL_LEFT) == GLFW_PRESS) {
        camera.processRotation(0.0f, 0.0f, -rollSpeed * deltaTime);
    }
    if (glfwGetKey(window, KeyMapping.ROLL_RIGHT) == GLFW_PRESS) {
        camera.processRotation(0.0f, 0.0f, rollSpeed * deltaTime);
    }

    if (glfwGetKey(window, KeyMapping.FPS) == GLFW_PRESS) {
        if (!fpsKeyPressed) {
            isFPS = !isFPS;
            if (isFPS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            firstMouse = true; // Reset mouse position on toggle
            fpsKeyPressed = true;
        }
    } else if (glfwGetKey(window, KeyMapping.FPS) == GLFW_RELEASE) {
        fpsKeyPressed = false; 
    }
}

void CameraController::processMouseInput(GLFWwindow* window, float deltaTime) {
    if (!isFPS) return;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - lastX) * mouseSensitivity;
    float yoffset = static_cast<float>(lastY - ypos) * mouseSensitivity;

    camera.processRotation(xoffset * deltaTime, yoffset * deltaTime, 0.0f); // No roll adjustment here

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);
}