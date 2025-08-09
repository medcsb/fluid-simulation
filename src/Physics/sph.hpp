#ifndef SPH_SOLVER_HPP
#define SPH_SOLVER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <vector>
#include <array>
#include <algorithm>
#include <unordered_map>
// accumulate
#include <numeric>

struct Particle{
    glm::vec3 position;
    glm::vec3 velocity;
};

struct GridCoord {
    int x, y, z;
    bool operator==(const GridCoord& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct GridCoordHash {
    std::size_t operator()(const GridCoord& coord) const {
        return (std::hash<int>()(coord.x) ^ 
                (std::hash<int>()(coord.y) << 1)) >> 1 ^ 
                (std::hash<int>()(coord.z) << 1);
    }
};

class SPHSolver {
public:

    glm::vec3 boxPos = glm::vec3(0.0f);
    glm::vec3 boxSize = glm::vec3(1.0f);
    float bounce = 0.5f;

    std::unordered_map<GridCoord, std::vector<size_t>, GridCoordHash> grid;
    std::vector<Particle> particles;
    std::vector<float> densities;
    std::vector<float> pressures;
    std::vector<glm::vec3> forces;

    float radius = 0.05f;
    float h = 2.0f * radius; 
    float h2 = h * h;
    float restDensity = 1000.0f;
    float mass = restDensity * (4.0f / 3.0f) * glm::pi<float>() * std::pow(radius, 3);
    float pressure_multiplier = 0.02f;
    float viscosity = 0.01f;
    float gravity_m = -0.1f; // Gravity acceleration in m/s^2

    SPHSolver() {}
    ~SPHSolver() {}

    void update(float dt);
    void spawnParticles();
    void spawnRandom();
    void reset();

    float getAverageDensity() const {
        return std::accumulate(densities.begin(), densities.end(), 0.0f) / densities.size();
    }

private:
    void builGrid();
    void computeDensityPressure();
    void computeForces();
    void integrate(float dt);

    GridCoord getCellCord(const glm::vec3& position) const;
    std::vector<uint32_t> getNeighbours(uint32_t idx) const;
    
    // Kernel functions
    float poly6_kernel(float r2) const;
    glm::vec3 spiky_grad(const glm::vec3& r, float rlen) const;
    float visc_lap(float rlen) const;
};

#endif // SPH_SOLVER_HPP