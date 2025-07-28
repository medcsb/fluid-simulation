#ifndef SPH_SOLVER_HPP
#define SPH_SOLVER_HPP

#include <glm/glm.hpp>

#include <vector>

const float GRAVITY = -9.81f;


struct Particle{
    glm::vec3 position;
    glm::vec3 velocity;
};

class SPHSolver {

public:
    std::vector<Particle> particles;

    float radius = 0.1f;

    SPHSolver() = default;
    ~SPHSolver() = default;


    void update(float dt);
    void reset();
    std::vector<glm::vec3> spawnParticles();

private:
    void resolveCollisions();
    void resolveWallCollisions();
    
};

#endif // SPH_SOLVER_HPP