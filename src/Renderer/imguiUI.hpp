#ifndef IMGUIUI_HPP
#define IMGUIUI_HPP

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "scene.hpp"

#include <GLFW/glfw3.h>

#include <string>

class ImguiUI {
private:
    GLFWwindow* window;
public:
    ImguiUI() {};
    ~ImguiUI();

    void init(GLFWwindow* win, const std::string& glsl_version);

    void beginRender();
    void render() {ImGui::Render();}
    void endRender();

    void mainInfoBoard(uint32_t& sceneSelector, std::vector<Scene>& scenes, bool& shadowsOn);
    void simpleScene(Scene& scene, Camera& camera, CameraController& cameraController, float& gamma, bool& isPerspective, bool& showDepth);

private:
    void transforms(std::vector<Model>& models);
    void cameraConfig(Camera& camera, CameraController& cameraController);
    void lightConfig(std::vector<Model>& models, uint32_t lightIdx);
};

#endif // IMGUIUI_HPP