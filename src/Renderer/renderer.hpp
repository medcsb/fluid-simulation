#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// using OpenGL 4.6
const int OPENGL_VERSION_MAJOR = 4;
const int OPENGL_VERSION_MINOR = 6;


class Renderer {
private:
    GLFWwindow* window;
    int width = 800;
    int height = 600;

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

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
};

#endif // RENDERER_HPP