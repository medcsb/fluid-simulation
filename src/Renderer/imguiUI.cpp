#include "imguiUI.hpp"


ImguiUI::~ImguiUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImguiUI::init(GLFWwindow* window, const std::string& glsl_version, SPHSolver* sphSolver) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    this->sphSolver = sphSolver;

    // Setup Dear ImGui flags
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Navigation

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    std::string glsl_version_str = "#version " + glsl_version;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version_str.c_str());
}

void ImguiUI::beginRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImguiUI::endRender() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImguiUI::mainInfoBoard(uint32_t& sceneSelector, std::vector<Scene>& scenes, bool& shadowsOn) {
    ImGui::Begin("Renderer Info");
    ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Current GPU :%s", glGetString(GL_RENDERER));
    ImGui::Text("vendor: %s", glGetString(GL_VENDOR));
    ImGui::Checkbox("Shadows On", &shadowsOn);
    // Scene Selector
    if (!scenes.empty()) {
        std::vector<const char*> sceneNames;
        sceneNames.reserve(scenes.size());
        for (const auto& scene : scenes) {
            sceneNames.push_back(scene.name.c_str());
        }
        ImGui::Combo("Select Scene", reinterpret_cast<int*>(&sceneSelector), sceneNames.data(), static_cast<int>(sceneNames.size()));
    } else {
        ImGui::Text("No scenes available.");
    }
    ImGui::End();
}

void ImguiUI::simpleScene(Scene& scene, Camera& camera, CameraController& cameraController, float& gamma, bool& isPerspective, bool& showDepth) {
    ImGui::Begin("Scence Controls");
    ImGui::Checkbox("Show Depth", &showDepth);
    ImGui::Checkbox("Perspective View", &isPerspective);
    ImGui::SliderFloat("Gamma", &gamma, 0.1f, 3.0f, "%.1f");

    transforms(scene);
    cameraConfig(camera, cameraController);
    lightConfig(scene.getModels(), scene.LightModelIdx);

    ImGui::End();
}

void ImguiUI::transforms(Scene& scene) {
    auto& models = scene.getModels();
    if (!ImGui::CollapsingHeader("Transforms")) return;
    if (models.empty()) {
        ImGui::Text("No models available.");
        return;
    }
    for (size_t i = 0; i < models.size(); ++i) {
        Model& model = models[i];

        // special case for particle model
        if (model.name == "particle") {
            ImGui::PushID(i);
            if (ImGui::CollapsingHeader(model.name.c_str())) {
                // text showing the number of particles
                ImGui::Text("Number of Particles: %zu", sphSolver->particles.size());
                ImGui::DragFloat("Radius", &model.getRadius(), 0.01f, 0.01f, 1.0f);
                // button to spawn particles
                if (ImGui::Button("Spawn Particles")) sphSolver->spawnParticles();
                if (ImGui::Button("Spawn Random Particles")) sphSolver->spawnRandom();
                if (ImGui::Button("Clear Particles")) sphSolver->reset();
            }
            ImGui::PopID();
            continue;
        }
        // special case sphere model
        if (model.name == "sphere") {
            // we can change the radius and resolution of the sphere
            ImGui::PushID(i);

            if (ImGui::CollapsingHeader(model.name.c_str())) {
                ImGui::DragFloat3("Position", glm::value_ptr(model.getTransform().translationVec), 0.01f);
                ImGui::ColorEdit3("Color", glm::value_ptr(model.getColor()));
                ImGui::DragFloat("Radius", &model.getRadius(), 0.01f, 0.01f, 1.0f);
                bool radiusChanged = ImGui::IsItemDeactivatedAfterEdit();
                ImGui::DragInt("Resolution", reinterpret_cast<int*>(&model.getResolution()), 1, 3, 100);
                bool resolutionChanged = ImGui::IsItemDeactivatedAfterEdit();

                // if radius or resolution changed, update the sphere model
                if (radiusChanged || resolutionChanged) {
                    model.vertices.clear();
                    model.indices.clear();
                    model.simpleSphere();
                    Buffer& buffer = scene.getBuffers()[scene.bufferMap.at(model.name)];
                    buffer.unbindVAO();
                    VAOConfigInfo config{};
                    config.sizeOfVertex = sizeof(Vertex);
                    config.sizeOfVertexData = model.vertices.size() * sizeof(Vertex);
                    config.verticesCount = model.vertices.size();
                    config.indexCount = model.indices.size();
                    config.attributes = {
                        {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
                        {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
                        {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
                    };
                    config.useEBO = true;
                    config.drawMode = GL_TRIANGLES;
                    buffer.init(model.vertices.data(), model.indices.data(), config);
                }
            }

            ImGui::PopID();
            continue;
        }
        ImGui::PushID(i);
        if (ImGui::CollapsingHeader(model.name.c_str())) {
            ImGui::DragFloat3("Position", glm::value_ptr(model.getTransform().translationVec), 0.01f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(model.getTransform().rotationVec), 0.1f);
            ImGui::DragFloat3("Scale", glm::value_ptr(model.getTransform().scaleVec), 0.01f);
            ImGui::ColorEdit3("Color", glm::value_ptr(model.getColor()));
        }
        ImGui::PopID();
    }
}

void ImguiUI::cameraConfig(Camera& camera, CameraController& cameraController) {
    if (!ImGui::CollapsingHeader("Camera Config")) return;
    ImGui::DragFloat("Camera Speed", &cameraController.getMovementSpeed(), 2.0f, 5.0f, 50.0f);
    ImGui::DragFloat("Roll Speed", &cameraController.getRollSpeed(), 0.01f, 0.1f, 10.0f);
    ImGui::DragFloat("Mouse Sensitivity", &cameraController.getMouseSensitivity(), 0.01f, 0.5f, 0.01f);
    ImGui::DragFloat("Camera FOV", &camera.getFov(), 0.1f, 1.0f, 90.0f);
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", camera.getFront().x, camera.getFront().y, camera.getFront().z);
    ImGui::Text("Camera Up: (%.2f, %.2f, %.2f)", camera.getUp().x, camera.getUp().y, camera.getUp().z);
    ImGui::Text("Camera Right: (%.2f, %.2f, %.2f)", camera.getRight().x, camera.getRight().y, camera.getRight().z);
    ImGui::Text("Camera Yaw: %.2f", camera.getYaw());
    ImGui::Text("Camera Pitch: %.2f", camera.getPitch());
    ImGui::Text("Camera Roll: %.2f", camera.getRoll());
}

void ImguiUI::lightConfig(std::vector<Model>& models, uint32_t LightModelIdx) {
    if (!ImGui::CollapsingHeader("Light Config")) return;
    if (LightModelIdx >= models.size()) {
        ImGui::Text("No light model available.");
        return;
    }
    Model& lightModel = models[LightModelIdx];
    ImGui::ColorEdit3("Light Color", glm::value_ptr(lightModel.getColor()));
    ImGui::DragFloat("Ambient Strength", &lightModel.light.ambientStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Specular Strength", &lightModel.light.specularStrength, 0.1f, 0.0f, 10.0f);
    ImGui::DragInt("Specular Power", &lightModel.light.specularPower, 1, 1, 360);
    ImGui::DragFloat("Opaque Value", &lightModel.getOpaqueVal(), 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Attenuation Factor", &lightModel.light.attenuationFactor, 0.01f, 0.0f, 10.0f);
}

