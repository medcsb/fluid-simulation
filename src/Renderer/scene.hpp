#ifndef SCENE_HPP
#define SCENE_HPP

#include "model.hpp"
#include "shader.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "cameraController.hpp"

#include <unordered_map>

struct Renderable {
    uint32_t modelIdx;
    uint32_t shaderIdx;
    uint32_t bufferIdx;
};

class Scene {
public:
    std::string name;

    uint32_t sceneIdx;
    uint32_t lightIdx;
    uint32_t shadowIdx;

    std::vector<Model> models;
    std::vector<Shader> shaders;
    std::vector<Buffer> buffers;
    std::vector<Renderable> renderables;

    // these are only used for init functions not render loops
    std::unordered_map<std::string, uint32_t> modelMap;
    std::unordered_map<std::string, uint32_t> shaderMap;
    std::unordered_map<std::string, uint32_t> bufferMap;
    std::unordered_map<std::string, uint32_t> renderableMap;

    Camera camera;
    CameraController cameraController{camera};

    Scene() {};
    ~Scene() {};

    void render(GLuint shadowMap);
    void clearSceneData();

    void emptyScene();
    void initExampleScene1();
    void floorScene();
    void sphScene();

    std::vector<Model>& getModels() {return models;}
    std::vector<Shader>& getShaders() {return shaders;}
    std::vector<Buffer>& getBuffers() {return buffers;}
    std::vector<Renderable>& getRenderables() {return renderables;}

    Model& getModelByName(const std::string& name) {
        return models[modelMap.at(name)];
    }
    Shader& getShaderByName(const std::string& name) {
        return shaders[shaderMap.at(name)];
    }
    Buffer& getBufferByName(const std::string& name) {
        return buffers[bufferMap.at(name)];
    }
    Renderable& getRenderableByName(const std::string& name) {
        return renderables[renderableMap.at(name)];
    }

private:

    // floor scene
    void initFloorSceneShaders();
    void initFloorSceneModels();
    void initFloorSceneBuffers();
    void initFloorSceneRenderables();

    // example scene 1
    void initScene1Models();
    void initScene1Shaders();
    void initScene1Buffers();
    void initScene1Renderables();
};

#endif // SCENE_HPP