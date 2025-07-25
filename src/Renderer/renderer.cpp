#include "renderer.hpp"

#include <stdexcept>
#include <iostream>
#include <string>


Renderer::Renderer() {}
Renderer::~Renderer() {
    for (auto& buffer : buffers) {
        buffer.cleanup();
    }

    for (auto& shader : shaders) {
        shader.deleteProgram();
    }

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
    initShaders();
    initModels();
    initBuffers();
    camera.init(static_cast<float>(width), static_cast<float>(height));
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    // gamma correction
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE); // Enable face culling
    glCullFace(GL_BACK); // Cull back faces
    glEnable(GL_BLEND); // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    frameTime = static_cast<float>(glfwGetTime());
}

void Renderer::render() {

    frameTime = static_cast<float>(glfwGetTime()) - lastFrame;
    lastFrame = static_cast<float>(glfwGetTime());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Info");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("OpenGL Version: %d.%d", OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
    // toggle for depth
    ImGui::Checkbox("Show Depth", &showDepth);
    ImGui::End();
    //ImGui::Begin("Model Editor");
    // vec3 silder for the 3 vertices of the triangle
    //for (size_t i = 0; i < model.getVertices().size(); ++i) {
    //    ImGui::SliderFloat2(("Vertex " + std::to_string(i) + " Position").c_str(), &model.getVertices()[i].pos.x, -1.0f, 1.0f);
    //    ImGui::ColorEdit3(("Vertex " + std::to_string(i) + " Color").c_str(), &model.getVertices()[i].color.x, ImGuiColorEditFlags_NoInputs);
    //    ImGui::SliderFloat2(("Vertex " + std::to_string(i) + " TexCoord").c_str(), &model.getVertices()[i].texCoord.x, 0.0f, 1.0f);
    //}
    //ImGui::End();

    ImGui::Begin("Transform Editor");
    ImGui::Text("CUBE");
    ImGui::DragFloat3("cube pos", glm::value_ptr(models[0].getTransform().translationVec), 0.01f);
    ImGui::DragFloat3("cube rot", glm::value_ptr(models[0].getTransform().rotationVec), 1.0f);
    ImGui::DragFloat3("cube scale", glm::value_ptr(models[0].getTransform().scaleVec), 0.01f, 0.1f, 10.0f);
    ImGui::ColorEdit3("cube color", glm::value_ptr(models[0].getColor()), ImGuiColorEditFlags_NoInputs);
    ImGui::Text("FLOOR");
    ImGui::DragFloat3("Position", glm::value_ptr(models[2].getTransform().translationVec), 0.01f);
    ImGui::DragFloat3("Rotation", glm::value_ptr(models[2].getTransform().rotationVec), 1.0f);
    ImGui::DragFloat3("Scale", glm::value_ptr(models[2].getTransform().scaleVec), 0.01f, 0.1f, 10.0f);
    ImGui::ColorEdit3("color", glm::value_ptr(models[2].getColor()), ImGuiColorEditFlags_NoInputs);
    ImGui::Text("DRAGON");
    ImGui::DragFloat3("dragon pos", glm::value_ptr(models[3].getTransform().translationVec), 0.01f);
    ImGui::DragFloat3("dragon rot", glm::value_ptr(models[3].getTransform().rotationVec), 1.0f);
    ImGui::DragFloat3("dragon scale", glm::value_ptr(models[3].getTransform().scaleVec), 0.01f, 0.1f, 10.0f);
    ImGui::ColorEdit3("dragon color", glm::value_ptr(models[3].getColor()), ImGuiColorEditFlags_NoInputs);
    
    ImGui::End();

    // light controls

    ImGui::Begin("Light Editor");
    ImGui::ColorEdit3("Color", glm::value_ptr(models[1].getColor()), ImGuiColorEditFlags_NoInputs);
    ImGui::DragFloat("gamma", &gamma, 0.01f, 1.0f, 3.0f);
    ImGui::DragFloat("Ambient Strength", &models[1].light.ambientStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Specular Strength", &models[1].light.specularStrength, 0.1f, 0.0f, 10.0f);
    ImGui::DragInt("Specular Power", &models[1].light.specularPower, 1.0f, 1, 360);
    ImGui::DragFloat("opaque Value", &models[1].getOpaqueVal(), 0.01, 0.0, 1.0);
    ImGui::DragFloat("Attenuation Factor", &models[1].light.attenuationFactor, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat3("Position", glm::value_ptr(models[1].getTransform().translationVec), 0.01f);
    ImGui::DragFloat3("Rotation", glm::value_ptr(models[1].getTransform().rotationVec), 1.0f);
    ImGui::DragFloat3("Scale", glm::value_ptr(models[1].getTransform().scaleVec), 0.01f, 0.1f, 10.0f);
    ImGui::End();

    // camera controls
    ImGui::Begin("Camera Controls");
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", camera.getFront().x, camera.getFront().y, camera.getFront().z);
    ImGui::Text("Camera Up: (%.2f, %.2f, %.2f)", camera.getUp().x, camera.getUp().y, camera.getUp().z);
    ImGui::Text("Camera Right: (%.2f, %.2f, %.2f)", camera.getRight().x, camera.getRight().y, camera.getRight().z);
    ImGui::Text("Camera Yaw: %.2f", camera.getYaw());
    ImGui::Text("Camera Pitch: %.2f", camera.getPitch());
    ImGui::Text("Camera Roll: %.2f", camera.getRoll());
    ImGui::DragFloat("Camera Speed", &cameraController.getMovementSpeed(), 2.0f, 5.0f, 50.0f);
    ImGui::DragFloat("Roll Speed", &cameraController.getRollSpeed(), 0.01f, 0.1f, 10.0f);
    ImGui::DragFloat("Mouse Sensitivity", &cameraController.getMouseSensitivity(), 0.01f, 0.5f, 0.01f);
    ImGui::DragFloat("Camera FOV", &camera.getFov(), 0.1f, 1.0f, 90.0f);
    // add toggle to switch between prespective and orthographic projection
    ImGui::Checkbox("Perspective/Orthographic", &isPerspective);
    if (isPerspective) {
        camera.updateProjectionMatrix(static_cast<float>(width), static_cast<float>(height));
    } else {
        camera.updateOrthographicMatrix(0.1f, 100.0f);
    }
    camera.updateViewMatrix();
    ImGui::End();
    

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * model.getVertices().size(), model.getVertices().data());

    models[0].bindTexture();

    // cube
    shaders[0].use();
    shaders[0].setUniform("transform", UniformType::MAT4, models[0].transform.getTransformMatrix());
    shaders[0].setUniform("view", UniformType::MAT4, camera.getViewMatrix());
    shaders[0].setUniform("projection", UniformType::MAT4, camera.getProjectionMatrix());
    shaders[0].setUniform("lightColor", UniformType::VEC3, models[1].getColor());
    shaders[0].setUniform("lightPos", UniformType::VEC3, models[1].getTransform().translationVec);
    shaders[0].setUniform("viewPos", UniformType::VEC3, camera.getPosition());
    shaders[0].setUniform("color", UniformType::VEC3, models[0].getColor());
    shaders[0].setUniform("ambientStrength", UniformType::FLOAT, models[1].light.ambientStrength);
    shaders[0].setUniform("specularStrength", UniformType::FLOAT, models[1].light.specularStrength);
    shaders[0].setUniform("specularPower", UniformType::INT, models[1].light.specularPower);
    shaders[0].setUniform("attenuationFactor", UniformType::FLOAT, models[1].light.attenuationFactor);
    shaders[0].setUniform("useTexture", UniformType::BOOL, true);
    shaders[0].setUniform("gamma", UniformType::FLOAT, gamma);
    if (showDepth) shaders[0].setUniform("showDepth", UniformType::BOOL, true);
    else shaders[0].setUniform("showDepth", UniformType::BOOL, false);

    buffers[0].bind();
    buffers[0].draw();

    models[2].bindTexture();

    // floor
    shaders[0].use();
    shaders[0].setUniform("transform", UniformType::MAT4, models[2].transform.getTransformMatrix());
    shaders[0].setUniform("view", UniformType::MAT4, camera.getViewMatrix());
    shaders[0].setUniform("projection", UniformType::MAT4, camera.getProjectionMatrix());
    shaders[0].setUniform("lightColor", UniformType::VEC3, models[1].getColor());
    shaders[0].setUniform("lightPos", UniformType::VEC3, models[1].getTransform().translationVec);
    shaders[0].setUniform("viewPos", UniformType::VEC3, camera.getPosition());
    shaders[0].setUniform("color", UniformType::VEC3, models[2].getColor());
    shaders[0].setUniform("ambientStrength", UniformType::FLOAT, models[1].light.ambientStrength);
    shaders[0].setUniform("specularStrength", UniformType::FLOAT, models[1].light.specularStrength);
    shaders[0].setUniform("specularPower", UniformType::INT, models[1].light.specularPower);
    shaders[0].setUniform("attenuationFactor", UniformType::FLOAT, models[1].light.attenuationFactor);
    shaders[0].setUniform("useTexture", UniformType::BOOL, true);
    shaders[0].setUniform("gamma", UniformType::FLOAT, gamma);
    if (showDepth) shaders[0].setUniform("showDepth", UniformType::BOOL, true);
    else shaders[0].setUniform("showDepth", UniformType::BOOL, false);

    buffers[2].bind();
    buffers[2].draw();

    // dragon
    shaders[0].use();
    shaders[0].setUniform("transform", UniformType::MAT4, models[3].transform.getTransformMatrix());
    shaders[0].setUniform("view", UniformType::MAT4, camera.getViewMatrix());
    shaders[0].setUniform("projection", UniformType::MAT4, camera.getProjectionMatrix());
    shaders[0].setUniform("lightColor", UniformType::VEC3, models[1].getColor());
    shaders[0].setUniform("lightPos", UniformType::VEC3, models[1].getTransform().translationVec);
    shaders[0].setUniform("viewPos", UniformType::VEC3, camera.getPosition());
    shaders[0].setUniform("color", UniformType::VEC3, models[3].getColor());
    shaders[0].setUniform("ambientStrength", UniformType::FLOAT, models[1].light.ambientStrength);
    shaders[0].setUniform("specularStrength", UniformType::FLOAT, models[1].light.specularStrength);
    shaders[0].setUniform("specularPower", UniformType::INT, models[1].light.specularPower);
    shaders[0].setUniform("useTexture", UniformType::BOOL, false);
    shaders[0].setUniform("attenuationFactor", UniformType::FLOAT, models[1].light.attenuationFactor);
    if (showDepth) shaders[0].setUniform("showDepth", UniformType::BOOL, true);
    else shaders[0].setUniform("showDepth", UniformType::BOOL, false);

    buffers[3].bind();
    buffers[3].draw();

    // light
    shaders[1].use();
    shaders[1].setUniform("transform", UniformType::MAT4, models[1].transform.getTransformMatrix());
    shaders[1].setUniform("view", UniformType::MAT4, camera.getViewMatrix());
    shaders[1].setUniform("projection", UniformType::MAT4, camera.getProjectionMatrix());
    shaders[1].setUniform("lightColor", UniformType::VEC3, models[1].getColor());
    shaders[1].setUniform("opaqueVal", UniformType::FLOAT, models[1].getOpaqueVal());

    buffers[1].bind();  
    buffers[1].draw();
    

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
    cameraController.processKeyboardInput(window, frameTime);
    cameraController.processMouseInput(window, frameTime);
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

void Renderer::initShaders() {
    Shader shader{"../shaders/vert.glsl", "../shaders/frag.glsl"};
    shader.init();
    shaders.push_back(shader);

    Shader lightShader{"../shaders/lightvert.glsl", "../shaders/lightfrag.glsl"};
    lightShader.init();
    shaders.push_back(lightShader);
}

void Renderer::initModels() {
    Model cubeModel;
    cubeModel.texturedCube();
    cubeModel.getTransform().translationVec = glm::vec3(-1.5f, 0.0f, -1.5f);
    cubeModel.loadTexture("../assets/textures/wood_container.jpg");
    models.push_back(cubeModel);
    Model light;
    light.simpleCube();
    light.getTransform().translationVec = glm::vec3(1.0f, 2.0f, 0.0f);
    light.getTransform().scaleVec = glm::vec3(0.2f, 0.2f, 0.2f);
    models.push_back(light);
    Model floor;
    floor.simpleQuad();
    floor.loadTexture("../assets/textures/brick_wall.jpg");
    // scale texture coordinates to fit the floor
    for (auto& vertex : floor.getVertices()) {
        vertex.texCoord *= glm::vec2(10.0f, 10.0f); // Scale texture coordinates
    }
    floor.setTextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    floor.getTransform().translationVec = glm::vec3(0.0f, -0.5f, 0.0f);
    floor.getTransform().scaleVec = glm::vec3(10.0f, 10.0f, 10.0f);
    floor.getTransform().rotationVec = glm::vec3(90.0f, 0.0f, 0.0f);
    //floor.color = {0.2f, 0.2f, 0.2f};
    models.push_back(floor);
    Model dragon;
    dragon.loadModel("../assets/models/dragon.obj");
    models.push_back(dragon);
}

void Renderer::initBuffers() {
    // Buffer for normal models
    Buffer simpleBuffer;
    VAOConfigInfo simpleBufferConfig{};
    simpleBufferConfig.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
        {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
        {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
    };
    simpleBufferConfig.sizeOfVertex = sizeof(Vertex);
    simpleBufferConfig.sizeOfVertexData = simpleBufferConfig.sizeOfVertex * models[0].getVertices().size();
    simpleBufferConfig.verticesCount = models[0].getVertices().size();
    simpleBufferConfig.indexCount = models[0].getIndices().size();
    simpleBufferConfig.useEBO = true;
    simpleBufferConfig.drawMode = GL_TRIANGLES;

    simpleBuffer.init(models[0].getVertices().data(), models[0].getIndices().data(), simpleBufferConfig);
    buffers.push_back(simpleBuffer);

    // Buffer for light
    Buffer lightBuffer;
    VAOConfigInfo lightBufferConfig{};
    lightBufferConfig.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoTex), offsetof(VertexNoTex, pos)},
        {2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoTex), offsetof(VertexNoTex, normal)}
    };
    lightBufferConfig.sizeOfVertex = sizeof(VertexNoTex);
    lightBufferConfig.sizeOfVertexData = lightBufferConfig.sizeOfVertex * models[1].getVerticesNoTex().size();
    lightBufferConfig.verticesCount = models[1].getVerticesNoTex().size();
    lightBufferConfig.indexCount = models[1].getIndices().size();
    lightBufferConfig.useEBO = true;
    lightBufferConfig.drawMode = GL_TRIANGLES;
    lightBuffer.init(models[1].getVerticesNoTex().data(), models[1].getIndices().data(), lightBufferConfig);
    buffers.push_back(lightBuffer);

    // Buffer for floor
    Buffer floorBuffer;
    VAOConfigInfo floorBufferConfig{};
    floorBufferConfig.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
        {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
        {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
    };
    floorBufferConfig.sizeOfVertex = sizeof(Vertex);
    floorBufferConfig.sizeOfVertexData = floorBufferConfig.sizeOfVertex * models[2].getVertices().size();
    floorBufferConfig.verticesCount = models[2].getVertices().size();
    floorBufferConfig.indexCount = models[2].getIndices().size();
    floorBufferConfig.useEBO = true;
    floorBufferConfig.drawMode = GL_TRIANGLES;
    floorBuffer.init(models[2].getVertices().data(), models[2].getIndices().data(), floorBufferConfig);
    buffers.push_back(floorBuffer);

    // buffer for dragon
    Buffer dragonBuffer;
    VAOConfigInfo dragonBufferConfig{};
    dragonBufferConfig.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
        {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
        {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
    };
    dragonBufferConfig.sizeOfVertex = sizeof(Vertex);
    dragonBufferConfig.sizeOfVertexData = dragonBufferConfig.sizeOfVertex * models[3].getVertices().size();
    dragonBufferConfig.verticesCount = models[3].getVertices().size();
    dragonBufferConfig.indexCount = models[3].getIndices().size();
    dragonBufferConfig.useEBO = true;
    dragonBufferConfig.drawMode = GL_TRIANGLES;

    dragonBuffer.init(models[3].getVertices().data(), models[3].getIndices().data(), dragonBufferConfig);
    buffers.push_back(dragonBuffer);
}

void addCubeMap(std::vector<std::string> faces) {

}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}