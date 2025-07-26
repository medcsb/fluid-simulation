#include "scene.hpp"


void Scene::initExampleScene1() {
    initScene1Shaders();
    initScene1Models();
    initScene1Buffers();
    initScene1Renderables();
}

void Scene::initScene1Shaders() {
    Shader simpleShader("../shaders/simple.vert", "../shaders/simple.frag");
    simpleShader.init();
    simpleShader.setName("simple");
    shaders.push_back(simpleShader);
    shaderMap["simple"] = shaders.size() - 1;

    Shader lightShader("../shaders/light.vert", "../shaders/light.frag");
    lightShader.init();
    lightShader.setName("light");
    shaders.push_back(lightShader);
    shaderMap["light"] = shaders.size() - 1;

    Shader shadowShader("../shaders/shadow.vert", "../shaders/shadow.frag");
    shadowShader.init();
    shadowShader.setName("shadow");
    shaders.push_back(shadowShader);
    shaderMap["shadow"] = shaders.size() - 1;
    shadowIdx = shaders.size() - 1;
}

void Scene::initScene1Models() {
    Model cubeModel;
    cubeModel.name = "cube";
    cubeModel.isTextured = true;
    cubeModel.texturedCube();
    cubeModel.getTransform().translationVec = glm::vec3(-1.5f, 0.0f, -1.5f);
    cubeModel.loadTexture("../assets/textures/wood_container.jpg");
    models.push_back(cubeModel);
    modelMap["cube"] = models.size() - 1;

    Model light;
    light.name = "light";
    light.isTextured = false;
    light.simpleCube();
    light.getTransform().translationVec = glm::vec3(1.0f, 2.0f, 0.0f);
    light.getTransform().scaleVec = glm::vec3(0.2f, 0.2f, 0.2f);
    models.push_back(light);
    lightIdx = models.size() - 1;
    modelMap["light"] = models.size() - 1;

    Model floor;
    floor.name = "floor";
    floor.isTextured = true;
    floor.simpleQuad();
    floor.loadTexture("../assets/textures/brick_wall.jpg");
    for (auto& vertex : floor.getVertices()) {
        vertex.texCoord *= glm::vec2(10.0f, 10.0f); // Scale texture coordinates
    }
    floor.setTextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    floor.getTransform().translationVec = glm::vec3(0.0f, -0.5f, 0.0f);
    floor.getTransform().scaleVec = glm::vec3(10.0f, 10.0f, 10.0f);
    floor.getTransform().rotationVec = glm::vec3(90.0f, 0.0f, 0.0f);
    models.push_back(floor);
    modelMap["floor"] = models.size() - 1;

    Model dragon;
    dragon.name = "dragon";
    dragon.isTextured = false;
    dragon.loadModel("../assets/models/dragon.obj");
    models.push_back(dragon);
    modelMap["dragon"] = models.size() - 1;
}

void Scene::initScene1Buffers() {
    // Buffer for cube
    Buffer simpleBuffer;
    VAOConfigInfo simpleBufferConfig{};
    simpleBufferConfig.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, pos)},
        {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
        {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord)}
    };
    simpleBufferConfig.sizeOfVertex = sizeof(Vertex);
    simpleBufferConfig.sizeOfVertexData = simpleBufferConfig.sizeOfVertex * getModelByName("cube").getVertices().size();
    simpleBufferConfig.verticesCount = getModelByName("cube").getVertices().size();
    simpleBufferConfig.indexCount = getModelByName("cube").getIndices().size();
    simpleBufferConfig.useEBO = true;
    simpleBufferConfig.drawMode = GL_TRIANGLES;

    simpleBuffer.init(getModelByName("cube").getVertices().data(), getModelByName("cube").getIndices().data(), simpleBufferConfig);
    buffers.push_back(simpleBuffer);
    bufferMap["cube"] = buffers.size() - 1;

    // Buffer for light
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

    // Buffer for floor
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

    // Buffer for dragon
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