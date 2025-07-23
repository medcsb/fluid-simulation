#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "cameraController.hpp"
#include "buffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "stb_image.h"

#include <iostream>

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

    float frameTime = 0.0f;
    float lastFrame = 0.0f;

    uint32_t VBO, VAO, EBO;
    uint32_t texture_wall, texture_statue;
    
    Camera camera;
    CameraController cameraController{camera};
    std::vector<Model> models;
    std::vector<Shader> shaders;
    std::vector<Buffer> buffers;

    // cameara controller ImGui
    bool isPerspective = true;
    bool showDepth = false;

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
    void initImGui();
    void initShaders();
    void initModels();
    void initBuffers();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
};

#endif // RENDERER_HPP