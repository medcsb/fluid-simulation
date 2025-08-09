#include "scene.hpp"

void Scene::emptyScene() {
    clearSceneData();
    name = "Empty Scene";
}

void Scene::initSphDemo(SPHSolver& sphSolver) {
    clearSceneData();
    initSphDemoShaders();
    initSphDemoModels();
    initSphDemoBuffers();
    initSphDemoRenderables();
    name = "SPH Demo";
}

void Scene::floorScene() {
    clearSceneData();
    initFloorSceneShaders();
    initFloorSceneModels();
    initFloorSceneBuffers();
    initFloorSceneRenderables();
    name = "Floor Scene";
}

void Scene::initExampleScene1() {
    clearSceneData();
    initScene1Shaders();
    initScene1Models();
    initScene1Buffers();
    initScene1Renderables();
    name = "Example Scene 1";
}

void Scene::sphScene() {
    clearSceneData();
    initSphShaders();
    initSphModels();
    initSphBuffers();
    initSphRenderables();
    name = "SPH Scene";
}

void Scene::clearSceneData() {
    models.clear();
    shaders.clear();
    buffers.clear();
    renderables.clear();
    modelMap.clear();
    shaderMap.clear();
    bufferMap.clear();
    renderableMap.clear();
}

void Scene::initSphShaders() {
    initSimpleShader();
    initLightShader();
    initShadowShader();
}

void Scene::initSphModels() {
    initFloorModel();
    initLightModel();
    initSphereModel();
}

void Scene::initSphBuffers() {
    initFloorBuffer();
    initLightBuffer();
    initSphereBuffer();
}

void Scene::initSphRenderables() {
    Renderable floorRenderable{modelMap["floor"], shaderMap["simple"], bufferMap["floor"]};
    renderables.push_back(floorRenderable);
    renderableMap["floor"] = renderables.size() - 1;

    Renderable lightRenderable{modelMap["light"], shaderMap["light"], bufferMap["light"]};
    renderables.push_back(lightRenderable);
    renderableMap["light"] = renderables.size() - 1;

    Renderable sphereRenderable{modelMap["sphere"], shaderMap["simple"], bufferMap["sphere"]};
    renderables.push_back(sphereRenderable);
    renderableMap["sphere"] = renderables.size() - 1;
}

void Scene::initScene1Shaders() {
    initSimpleShader();
    initLightShader();
    initShadowShader();
}

void Scene::initScene1Models() {
    initCubeModel();
    models.back().getTransform().translationVec = glm::vec3(-1.5f, 0.5f, -1.5f);

    initLightModel();
    initFloorModel();
    initDragonModel();
    getModelByName("dragon").getTransform().translationVec = glm::vec3(0.0f, 0.5f, 0.0f);
}

void Scene::initScene1Buffers() {
    initCubeBuffer();
    initLightBuffer();
    initFloorBuffer();
    initDragonBuffer();
}

void Scene::initScene1Renderables() {
    Renderable cubeRenderable{modelMap["cube"], shaderMap["simple"], bufferMap["cube"]};
    renderables.push_back(cubeRenderable);
    renderableMap["cube"] = renderables.size() - 1;

    Renderable lightRenderable{modelMap["light"], shaderMap["light"], bufferMap["light"]};
    renderables.push_back(lightRenderable);
    renderableMap["light"] = renderables.size() - 1;

    Renderable floorRenderable{modelMap["floor"], shaderMap["simple"], bufferMap["floor"]};
    renderables.push_back(floorRenderable);
    renderableMap["floor"] = renderables.size() - 1;

    Renderable dragonRenderable{modelMap["dragon"], shaderMap["simple"], bufferMap["dragon"]};
    renderables.push_back(dragonRenderable);
    renderableMap["dragon"] = renderables.size() - 1;
}

void Scene::initFloorSceneShaders() {
    initSimpleShader();
    initLightShader();
    initShadowShader();
}

void Scene::initFloorSceneModels() {
    initFloorModel();
    initLightModel();
}

void Scene::initFloorSceneBuffers() {
    initFloorBuffer();
    initLightBuffer();
}

void Scene::initFloorSceneRenderables() {
    Renderable floorRenderable{modelMap["floor"], shaderMap["simple"], bufferMap["floor"]};
    renderables.push_back(floorRenderable);
    renderableMap["floor"] = renderables.size() - 1;
    Renderable lightRenderable{modelMap["light"], shaderMap["light"], bufferMap["light"]};
    renderables.push_back(lightRenderable);
    renderableMap["light"] = renderables.size() - 1;
}

void Scene::initSphDemoShaders() {
    initSimpleShader();
    initLightShader();
    initShadowShader();
    initSphShader();
}

void Scene::initSphDemoModels() {
    initFloorModel();
    initLightModel();
    initParticleModel();
    initCubeModel();
    models.back().getTransform().translationVec = glm::vec3(0.0f, 0.6f, 0.0f);
    models.back().getTransform().scaleVec = glm::vec3(1.0f, 1.0f, 1.0f);
    for (auto& vertex: models.back().getVertices()) {
        vertex.texCoord *= glm::vec2(1.0f, 1.0f);
    }
}

void Scene::initSphDemoBuffers() {
    initFloorBuffer();
    initLightBuffer();
    initParticleBuffer();
    initCubeBuffer();
}

void Scene::initSphDemoRenderables() {
    Renderable floorRenderable{modelMap["floor"], shaderMap["simple"], bufferMap["floor"]};
    renderables.push_back(floorRenderable);
    renderableMap["floor"] = renderables.size() - 1;

    Renderable lightRenderable{modelMap["light"], shaderMap["light"], bufferMap["light"]};
    renderables.push_back(lightRenderable);
    renderableMap["light"] = renderables.size() - 1;

    Renderable particleRenderable{modelMap["particle"], shaderMap["sph"], bufferMap["particle"]};
    renderables.push_back(particleRenderable);
    renderableMap["particle"] = renderables.size() - 1;

    Renderable cubeRenderable{modelMap["cube"], shaderMap["simple"], bufferMap["cube"]};
    renderables.push_back(cubeRenderable);
    renderableMap["cube"] = renderables.size() - 1;
}