#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include "camera.hpp"

#include <GLFW/glfw3.h>

struct KeyMapping {
    int FORWARD = GLFW_KEY_W;
    int BACKWARD = GLFW_KEY_S;
    int LEFT = GLFW_KEY_A;
    int RIGHT = GLFW_KEY_D;
    int UP = GLFW_KEY_SPACE;
    int DOWN = GLFW_KEY_LEFT_ALT;
    int FPS = GLFW_KEY_F;
    int ROLL_LEFT = GLFW_KEY_Q;
    int ROLL_RIGHT = GLFW_KEY_E;
};

class CameraController {
private:
    Camera& camera;
    KeyMapping KeyMapping;
public:
    float movementSpeed{5.0f};
    float rollSpeed{10.0f};
    float mouseSensitivity{0.1f};
    float lastX{0.0f};
    float lastY{0.0f};
    bool firstMouse{true};
    bool isFPS{false};
    bool fpsKeyPressed{false};


    CameraController(Camera& cam) : camera(cam) {}

    void processKeyboardInput(GLFWwindow* window, float deltaTime);
    void processMouseInput(GLFWwindow* window, float deltaTime);

    float& getMovementSpeed() {return movementSpeed;}
    float& getMouseSensitivity() {return mouseSensitivity;}
    float& getRollSpeed() {return rollSpeed;}
};

#endif // CAMERA_CONTROLLER_HPP