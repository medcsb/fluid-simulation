#include "renderer.hpp"

#include <stdexcept>
#include <iostream>
#include <string>


Renderer::Renderer() {}
Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shader.deleteProgram();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::init() {
    initWindow();
    initOpenGL();
    initImGui();
    model.simpleQuad();
    initBuffers();
    model.loadTexture("../assets/textures/wood_container.jpg");
    shader.init();
}

void Renderer::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Info");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("OpenGL Version: %d.%d", OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
    ImGui::End();
    ImGui::Begin("Model Editor");
    // vec3 silder for the 3 vertices of the triangle
    for (size_t i = 0; i < model.getVertices().size(); ++i) {
        ImGui::SliderFloat2(("Vertex " + std::to_string(i) + " Position").c_str(), &model.getVertices()[i].pos.x, -1.0f, 1.0f);
        ImGui::ColorEdit3(("Vertex " + std::to_string(i) + " Color").c_str(), &model.getVertices()[i].color.x, ImGuiColorEditFlags_NoInputs);
        ImGui::SliderFloat2(("Vertex " + std::to_string(i) + " TexCoord").c_str(), &model.getVertices()[i].texCoord.x, 0.0f, 1.0f);
    }
    ImGui::End();

    ImGui::Begin("Transform Editor");
    ImGui::Text("Translation");
    ImGui::DragFloat3("Position", glm::value_ptr(model.getTransform().translationVec), 0.01f);
    ImGui::Text("Rotation");
    ImGui::DragFloat3("Rotation", glm::value_ptr(model.getTransform().rotationVec), 1.0f);
    ImGui::Text("Scale");
    ImGui::DragFloat3("Scale", glm::value_ptr(model.getTransform().scaleVec), 0.01f, 0.1f, 10.0f);
    ImGui::End();

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Model::Vertex) * model.getVertices().size(), model.getVertices().data());

    model.bindTexture();

    shader.use();
    shader.setUniform("transform", UniformType::MAT4, model.getTransform().getTransformMatrix());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Renderer::initWindow() {
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "SPH Fluid Simulation", nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);
}

void Renderer::initOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, Renderer::framebuffer_size_callback);
}

void Renderer::initImGui() {
    float min_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Navigation

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    int version = OPENGL_VERSION_MAJOR * 100 + OPENGL_VERSION_MINOR * 10;
    std::string glsl_version = "#version " + std::to_string(version);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

void Renderer::initBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Model::Vertex) * model.getVertices().size(), model.getVertices().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * model.getIndices().size(), model.getIndices().data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)offsetof(Model::Vertex, pos));
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)offsetof(Model::Vertex, color));
    glEnableVertexAttribArray(1);
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)offsetof(Model::Vertex, texCoord));
    glEnableVertexAttribArray(2);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}