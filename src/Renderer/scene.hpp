#ifndef SCENE_HPP
#define SCENE_HPP

#include "model.hpp"
#include "shader.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "cameraController.hpp"
#include "sph.hpp"

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
    uint32_t LightModelIdx;
    uint32_t shadowShaderIdx;

    std::vector<Model> models;
    std::vector<Shader> shaders;
    std::vector<Buffer> buffers;
    std::vector<Renderable> renderables;

    // these are only used for init functions not render loops
    std::unordered_map<std::string, uint32_t> modelMap;
    std::unordered_map<std::string, uint32_t> shaderMap;
    std::unordered_map<std::string, uint32_t> bufferMap;
    std::unordered_map<std::string, uint32_t> renderableMap;

    Scene() {};
    ~Scene() {};

    void render(GLuint shadowMap);
    void clearSceneData();
    void refreshBuffers(uint32_t modelIdx);

    void emptyScene();
    void initExampleScene1();
    void floorScene();
    void sphScene();
    void initSphDemo(SPHSolver& sphSolver);

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

    // SPH scene
    void initSphShaders();
    void initSphModels();
    void initSphBuffers();
    void initSphRenderables();

    void initSphDemoShaders();
    void initSphDemoModels();
    void initSphDemoBuffers();
    void initSphDemoRenderables();

    // utility functions

    // -------------------------------------------------
    // Shader initialization functions
    // -------------------------------------------------

    void initSimpleShader() {
        Shader simpleShader(std::string(SHADER_DIR) + "simple.vert", std::string(SHADER_DIR) + "simple.frag");
        simpleShader.init();
        simpleShader.setName("simple");
        shaders.push_back(simpleShader);
        shaderMap["simple"] = shaders.size() - 1;
    }

    void initLightShader() {
        Shader lightShader(std::string(SHADER_DIR) + "light.vert", std::string(SHADER_DIR) + "light.frag");
        lightShader.init();
        lightShader.setName("light");
        shaders.push_back(lightShader);
        shaderMap["light"] = shaders.size() - 1;
    }

    void initShadowShader() {
        Shader shadowShader(std::string(SHADER_DIR) + "shadow.vert", std::string(SHADER_DIR) + "shadow.frag");
        
        shadowShader.init();
        shadowShader.setName("shadow");
        shaders.push_back(shadowShader);
        shaderMap["shadow"] = shaders.size() - 1;
        shadowShaderIdx = shaders.size() - 1;
    }

    void initSphShader() {
        Shader sphShader(std::string(SHADER_DIR) + "sph.vert", std::string(SHADER_DIR) + "sph.frag");
        sphShader.init();
        sphShader.setName("sph");
        shaders.push_back(sphShader);
        shaderMap["sph"] = shaders.size() - 1;
    }

    // ------------------------------------------------
    // model initialization functions
    // ------------------------------------------------

    void initCubeModel() {
        Model cubeModel;
        cubeModel.name = "cube";
        cubeModel.isTextured = true;
        cubeModel.texturedCube();
        cubeModel.loadTexture(std::string(TEXTURES_DIR) + "tiled.jpg");
        models.push_back(cubeModel);
        modelMap["cube"] = models.size() - 1;
    }

    void initFloorModel() {
        Model floor;
        floor.name = "floor";
        floor.isTextured = true;
        floor.simpleQuad();
        floor.loadTexture(std::string(TEXTURES_DIR) + "brick_wall.jpg");
        for (auto& vertex : floor.getVertices()) {
            vertex.texCoord *= glm::vec2(10.0f, 10.0f); // Scale texture coordinates
        }
        floor.setTextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        floor.getTransform().translationVec = glm::vec3(0.0f, 0.0f, 0.0f);
        floor.getTransform().scaleVec = glm::vec3(10.0f, 10.0f, 10.0f);
        floor.getTransform().rotationVec = glm::vec3(90.0f, 0.0f, 0.0f);
        models.push_back(floor);
        modelMap["floor"] = models.size() - 1;
    }

    void initSphereModel() {
        Model sphereModel;
        sphereModel.name = "sphere";
        sphereModel.isTextured = false;
        sphereModel.simpleSphere();
        models.push_back(sphereModel);
        modelMap["sphere"] = models.size() - 1;
    }

    void initDragonModel() {
        Model dragonModel;
        dragonModel.name = "dragon";
        dragonModel.isTextured = false;
        dragonModel.loadModel(std::string(MODELS_DIR) + "dragon.obj");
        models.push_back(dragonModel);
        modelMap["dragon"] = models.size() - 1;
    }

    void initLightModel() {
        Model light;
        light.name = "light";
        light.isTextured = false;
        light.simpleCube();
        light.getTransform().translationVec = glm::vec3(1.0f, 2.0f, 0.5f);
        light.getTransform().scaleVec = glm::vec3(0.2f, 0.2f, 0.2f);
        models.push_back(light);
        LightModelIdx = models.size() - 1;
        modelMap["light"] = models.size() - 1;
    }

    void initParticleModel() {
        Model particleModel;
        particleModel.name = "particle";
        particleModel.isTextured = false;
        particleModel.particle();
        models.push_back(particleModel);
        modelMap["particle"] = models.size() - 1;
    }

    // ------------------------------------------------
    // Buffer initialization functions
    // ------------------------------------------------

    void initCubeBuffer() {
        Buffer cubeBuffer;
        VAOConfigInfo cubeConfig{};
        cubeConfig.attributes = {
            {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
            {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
            {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
        };
        cubeConfig.sizeOfVertex = sizeof(Vertex);
        cubeConfig.sizeOfVertexData = cubeConfig.sizeOfVertex * getModelByName("cube").getVertices().size();
        cubeConfig.verticesCount = getModelByName("cube").getVertices().size();
        cubeConfig.indexCount = getModelByName("cube").getIndices().size();
        cubeConfig.useEBO = true;
        cubeConfig.drawMode = GL_TRIANGLES;

        cubeBuffer.init(getModelByName("cube").getVertices().data(), getModelByName("cube").getIndices().data(), cubeConfig);
        buffers.push_back(cubeBuffer);
        bufferMap["cube"] = buffers.size() - 1;
    }

    void initFloorBuffer() {
        Buffer floorBuffer;
        VAOConfigInfo floorBufferConfig{};
        floorBufferConfig.attributes = {
            {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
            {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
            {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
        };
        floorBufferConfig.sizeOfVertex = sizeof(Vertex);
        floorBufferConfig.sizeOfVertexData = floorBufferConfig.sizeOfVertex * getModelByName("floor").getVertices().size();
        floorBufferConfig.verticesCount = getModelByName("floor").getVertices().size();
        floorBufferConfig.indexCount = getModelByName("floor").getIndices().size();
        floorBufferConfig.useEBO = true;
        floorBufferConfig.drawMode = GL_TRIANGLES;

        floorBuffer.init(getModelByName("floor").getVertices().data(), getModelByName("floor").getIndices().data(), floorBufferConfig);
        buffers.push_back(floorBuffer);
        bufferMap["floor"] = buffers.size() - 1;
    }

    void initLightBuffer() {
        Buffer lightBuffer;
        VAOConfigInfo lightBufferConfig{};
        lightBufferConfig.attributes = {
            {0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoTex), offsetof(VertexNoTex, pos)},
            {1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoTex), offsetof(VertexNoTex, normal)}
        };
        lightBufferConfig.sizeOfVertex = sizeof(VertexNoTex);
        lightBufferConfig.sizeOfVertexData = lightBufferConfig.sizeOfVertex * getModelByName("light").getVerticesNoTex().size();
        lightBufferConfig.verticesCount = getModelByName("light").getVerticesNoTex().size();
        lightBufferConfig.indexCount = getModelByName("light").getIndices().size();
        lightBufferConfig.useEBO = true;
        lightBufferConfig.drawMode = GL_TRIANGLES;

        lightBuffer.init(getModelByName("light").getVerticesNoTex().data(), getModelByName("light").getIndices().data(), lightBufferConfig);
        buffers.push_back(lightBuffer);
        bufferMap["light"] = buffers.size() - 1;
    }

    void initDragonBuffer() {
        Buffer dragonBuffer;
        VAOConfigInfo dragonBufferConfig{};
        dragonBufferConfig.attributes = {
            {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
            {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
            {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
        };
        dragonBufferConfig.sizeOfVertex = sizeof(Vertex);
        dragonBufferConfig.sizeOfVertexData = dragonBufferConfig.sizeOfVertex * getModelByName("dragon").getVertices().size();
        dragonBufferConfig.verticesCount = getModelByName("dragon").getVertices().size();
        dragonBufferConfig.indexCount = getModelByName("dragon").getIndices().size();
        dragonBufferConfig.useEBO = true;
        dragonBufferConfig.drawMode = GL_TRIANGLES;

        dragonBuffer.init(getModelByName("dragon").getVertices().data(), getModelByName("dragon").getIndices().data(), dragonBufferConfig);
        buffers.push_back(dragonBuffer);
        bufferMap["dragon"] = buffers.size() - 1;
    }

    void initSphereBuffer() {
        Buffer sphereBuffer;
        VAOConfigInfo sphereBufferConfig{};
        sphereBufferConfig.attributes = {
            {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
            {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
            {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
        };
        sphereBufferConfig.sizeOfVertex = sizeof(Vertex);
        sphereBufferConfig.sizeOfVertexData = sphereBufferConfig.sizeOfVertex * getModelByName("sphere").getVertices().size();
        sphereBufferConfig.verticesCount = getModelByName("sphere").getVertices().size();
        sphereBufferConfig.indexCount = getModelByName("sphere").getIndices().size();
        sphereBufferConfig.useEBO = true;
        sphereBufferConfig.drawMode = GL_TRIANGLES;

        sphereBuffer.init(getModelByName("sphere").getVertices().data(), getModelByName("sphere").getIndices().data(), sphereBufferConfig);
        buffers.push_back(sphereBuffer);
        bufferMap["sphere"] = buffers.size() - 1;
    }

    void initParticleBuffer() {
        Buffer particleBuffer;
        InstanceConfigInfo configInfo{};
        configInfo.staticAttribs = {
            {0, 3, GL_FLOAT, GL_FALSE, sizeof(PVertex), offsetof(PVertex, pos)},
            {1, 3, GL_FLOAT, GL_FALSE, sizeof(PVertex), offsetof(PVertex, normal)}
        };
        configInfo.instanceAttribs = {
            {2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleT), offsetof(ParticleT, pos)},
            {3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleT), offsetof(ParticleT, color)}
        };
        configInfo.sizeOfVertex = sizeof(PVertex);
        configInfo.sizeOfVertexData = configInfo.sizeOfVertex * getModelByName("particle").getPVertices().size();
        configInfo.sizeOfInstance = sizeof(ParticleT);
        configInfo.verticesCount = getModelByName("particle").getPVertices().size();
        configInfo.indexCount = getModelByName("particle").getIndices().size();
        configInfo.instancesCount = 10000; 
        configInfo.useEBO = true;
        configInfo.drawMode = GL_TRIANGLES;

        particleBuffer.initInstanced(
            getModelByName("particle").getPVertices().data(),
            nullptr,
            getModelByName("particle").getIndices().data(),
            configInfo
        );
        buffers.push_back(particleBuffer);
        bufferMap["particle"] = buffers.size() - 1;
    }

};

#endif // SCENE_HPP