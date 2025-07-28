#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "cameraController.hpp"
#include "buffer.hpp"
#include "imguiUI.hpp"
#include "scene.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "stb_image.h"

#include <iostream>
#include <memory>

// using OpenGL 4.6
const int OPENGL_VERSION_MAJOR = 4;
const int OPENGL_VERSION_MINOR = 5;

extern const char *vertexShaderSource;
extern const char *fragmentShaderSource;

class Renderer {
private:
    GLFWwindow* window;
    int width = 800;
    int height = 600;

    ImguiUI imguiUI;

    // stupid not used should be removed
    float frameTime = 0.0f;

    float gamma = 2.2f;

    GLuint shadowMapFBO;
    GLuint shadowMap;
    uint32_t shadow_width = 2048 * 2, shadow_height = 2048 * 2;
    glm::mat4 lightSpaceMatrix;

    uint32_t VBO, VAO, EBO;
    
    Camera camera;
    CameraController cameraController{camera};

    std::vector<Scene> scenes;
    uint32_t currentSceneIdx = 0;
    uint32_t sceneSelector = 0;

    // cameara controller ImGui
    bool isPerspective = true;
    bool showDepth = false;
    bool shadowsOn = false;

public:

    Renderer();
    ~Renderer();

    void init();
    void render();
    void processInput();

    bool shouldWindowClose() const {
        return glfwWindowShouldClose(window);
    }

private:
    void initWindow();
    void initOpenGL();
    void initScenes();
    void initShadowMap();
    void initRenderStuff();

    void renderShadowMap();

    void calculateLightSpaceMatrix();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
};

#endif // RENDERER_HPP