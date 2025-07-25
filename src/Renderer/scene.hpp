#ifndef SCENE_HPP
#define SCENE_HPP

#include "model.hpp"
#include "shader.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "cameraController.hpp"


class Scene {
private:
    std::vector<Model> models;
    std::vector<Shader> shaders;
    std::vector<Buffer> buffers;
    Camera camera;
    CameraController cameraController{camera};

    

public:
    Scene() {};
    ~Scene() {};

    void initExampleCube();
    void initExampleDragon();

};

#endif // SCENE_HPP