#ifndef SPH_SOLVER_HPP
#define SPH_SOLVER_HPP

#include <glm/glm.hpp>
// constants like M_PI are not defined in glm, so we need to include cmath
#include <cmath>
#include <vector>

const float GRAVITY = -9.81f;

struct Particle{
    glm::vec3 position;
    glm::vec3 velocity;
};

class SPHSolver {
public:
    std::vector<Particle> particles;
    std::vector<float> densities; 

    // Container transform properties
    glm::vec3 containerPosition = glm::vec3(0.0f);  // Center of the container
    glm::vec3 containerScale = glm::vec3(2.0f);     // Full size of container (not half-extents)

    float radius = 0.02f;

    SPHSolver() = default;
    ~SPHSolver() = default;

    void update(float dt);
    void reset();
    void spawnParticles();
    void spawnRandom();


    // Helper functions to get container bounds
    glm::vec3 getContainerMin() const {
        return containerPosition - containerScale * 0.5f;
    }
    
    glm::vec3 getContainerMax() const {
        return containerPosition + containerScale * 0.5f;
    }

private:
    void eulerIntegration(float dt);
    void computeDensity();
    void resolveCollisions();
    void resolveWallCollisions();

    float poly6_kernel(float r, float h) {
        if (r >= h || r < 0.0f) return 0.0f;
        const float PI = 3.14159265358979323846f;
        float factor = 315.0f / (64.0f * PI * pow(h, 9));
        return factor * pow(h * h - r * r, 3);
    }
};

#endif // SPH_SOLVER_HPP