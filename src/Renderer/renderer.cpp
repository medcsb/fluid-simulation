#include "renderer.hpp"

#include <stdexcept>
#include <iostream>
#include <string>


Renderer::Renderer() {}
Renderer::~Renderer() {
    for (auto& scene : scenes) {
        for (auto& shader : scene.getShaders()) shader.cleanup();
        for (auto& buffer : scene.getBuffers()) buffer.cleanup();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::init() {
    initWindow();
    initOpenGL();
    imguiUI.init(window, std::to_string(OPENGL_VERSION_MAJOR * 100 + OPENGL_VERSION_MINOR * 10));
    initScenes();
    initShadowMap();
    initRenderStuff();
    sceneSelector = 0;
    // move the camera a tiny bit up
}

void Renderer::render() {

    imguiUI.beginRender();
    imguiUI.mainInfoBoard(sceneSelector, scenes, shadowsOn);
    imguiUI.simpleScene(scenes[currentSceneIdx], camera, cameraController, gamma, isPerspective, showDepth);
    imguiUI.render();

    if (sceneSelector != currentSceneIdx) {
        currentSceneIdx = sceneSelector;
    }

    if (isPerspective) {
        camera.updateProjectionMatrix(static_cast<float>(width), static_cast<float>(height));
    } else {
        camera.updateOrthographicMatrix(0.1f, 100.0f);
    }
    camera.updateViewMatrix();

    Scene& currentScene = scenes[currentSceneIdx];
    std::vector<Model>& models = currentScene.getModels();
    std::vector<Shader>& shaders = currentScene.getShaders();
    std::vector<Buffer>& buffers = currentScene.getBuffers();
    std::vector<Renderable>& renderables = currentScene.getRenderables();

    if (!renderables.empty() && shadowsOn) renderShadowMap();
    if (!shadowsOn) {
        // reset shadow map texture
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glCullFace(GL_BACK);

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * model.getVertices().size(), model.getVertices().data());


    for (auto& obj : renderables) {
        Shader& shader = shaders[obj.shaderIdx];
        Buffer& buffer = buffers[obj.bufferIdx];
        Model& model = models[obj.modelIdx];
        if (model.isTextured) model.bindTexture();
        shader.use();
        shader.setUniform("transform", UniformType::MAT4, model.transform.getTransformMatrix());
        shader.setUniform("view", UniformType::MAT4, camera.getViewMatrix());
        shader.setUniform("projection", UniformType::MAT4, camera.getProjectionMatrix());
        shader.setUniform("lightColor", UniformType::VEC3, currentScene.getModels()[currentScene.LightModelIdx].getColor());
        if (shader.getName() == "simple") {
            shader.setUniform("lightSpaceMatrix", UniformType::MAT4, lightSpaceMatrix);
            shader.setUniform("lightPos", UniformType::VEC3, currentScene.getModels()[currentScene.LightModelIdx].getTransform().translationVec);
            shader.setUniform("viewPos", UniformType::VEC3, camera.getPosition());
            shader.setUniform("color", UniformType::VEC3, model.getColor());
            shader.setUniform("ambientStrength", UniformType::FLOAT, currentScene.getModels()[currentScene.LightModelIdx].light.ambientStrength);
            shader.setUniform("specularStrength", UniformType::FLOAT, currentScene.getModels()[currentScene.LightModelIdx].light.specularStrength);
            shader.setUniform("specularPower", UniformType::INT, currentScene.getModels()[currentScene.LightModelIdx].light.specularPower);
            shader.setUniform("attenuationFactor", UniformType::FLOAT, currentScene.getModels()[currentScene.LightModelIdx].light.attenuationFactor);
            shader.setUniform("useTexture", UniformType::BOOL, model.isTextured);
            shader.setUniform("gamma", UniformType::FLOAT, gamma);
            if (showDepth) shader.setUniform("showDepth", UniformType::BOOL, true);
            else shader.setUniform("showDepth", UniformType::BOOL, false);
            glUniform1i(glGetUniformLocation(shader.getID(), "texture1"), 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
            glUniform1i(glGetUniformLocation(shader.getID(), "shadowMap"), 1);
        } else if (shader.getName() == "light") {
            shader.setUniform("opaqueVal", UniformType::FLOAT, model.getOpaqueVal());
        }
        buffer.bind();
        buffer.draw();
    }

    imguiUI.endRender();

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

void Renderer::initScenes() {
    Scene emptyScene;
    emptyScene.emptyScene();
    scenes.push_back(emptyScene);
    Scene scene1;
    scene1.initExampleScene1();
    scenes.push_back(scene1);
    Scene floorScene;
    floorScene.floorScene();
    scenes.push_back(floorScene);
    Scene sphScene;
    sphScene.sphScene();
    scenes.push_back(sphScene);
}

void Renderer::initShadowMap() {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE); // No color buffer is drawn
    glReadBuffer(GL_NONE); // No color buffer is read
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::initRenderStuff() {
    camera.init(static_cast<float>(width), static_cast<float>(height));
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    // gamma correction
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE); // Enable face culling
    //glCullFace(GL_BACK); // Cull back faces
    glEnable(GL_BLEND); // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    frameTime = static_cast<float>(glfwGetTime());
}

void Renderer::renderShadowMap() {
    calculateLightSpaceMatrix();

    glCullFace(GL_FRONT);

    glViewport(0, 0, shadow_width, shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    Shader& shadowShader = scenes[currentSceneIdx].getShaders()[scenes[currentSceneIdx].shadowShaderIdx];

    shadowShader.use();
    shadowShader.setUniform("lightSpaceMatrix", UniformType::MAT4, lightSpaceMatrix);

    Scene& currentScene = scenes[currentSceneIdx];
    std::vector<Model>& models = currentScene.getModels();
    std::vector<Shader>& shaders = currentScene.getShaders();
    std::vector<Buffer>& buffers = currentScene.getBuffers();

    for (auto& obj: currentScene.getRenderables()) {
        Shader& shader = shaders[obj.shaderIdx];
        Buffer& buffer = buffers[obj.bufferIdx];
        Model& model = models[obj.modelIdx];
        if (shader.getName() != "simple") continue;

        shadowShader.setUniform("transform", UniformType::MAT4, model.transform.getTransformMatrix());
        buffer.bind();
        buffer.draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::calculateLightSpaceMatrix() {
    glm::vec3 lightPos = scenes[currentSceneIdx].getModels()[scenes[currentSceneIdx].LightModelIdx].getTransform().translationVec;
    float near_plane = 1.0f, far_plane = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}