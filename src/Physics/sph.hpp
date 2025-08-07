#ifndef SPH_SOLVER_HPP
#define SPH_SOLVER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <vector>
#include <array>
#include <algorithm>

const float GRAVITY = -9.81f;

struct Particle{
    glm::vec3 position;
    glm::vec3 velocity;
};

class SPHSolver {
public:

    float timeStep = 0.01f;
    float gravity_m = -1.0f;
    float max_vel = 1.0f;

    // gaz consant from the perfect gas law
    float GAS_CONSTANT = 1.0f;
    const float PI = glm::pi<float>();

    const uint32_t GRID_SIZE_X = 100;
    const uint32_t GRID_SIZE_Y = 100;
    const uint32_t GRID_SIZE_Z = 100;
    const glm::vec3 GRID_CENTER = glm::vec3(0.0f);

    float radius = 1.0f;
    float smoothingRadius = 2.0f * radius; 
    float restDensity = 2.5f;
    float mass = 0.03f;

    std::vector<std::vector<uint32_t>> grid;
    std::vector<Particle> particles;
    std::vector<float> densities;
    std::vector<float> pressures;
    std::vector<glm::vec3> forces;
    std::vector<glm::vec3> predictedPositions;

    // Container transform properties
    glm::vec3 containerPosition = glm::vec3(0.0f);  // Center of the container
    glm::vec3 containerScale = glm::vec3(2.0f);     // Full size of container (not half-extents)

    SPHSolver() {
        grid.resize(GRID_SIZE_X * GRID_SIZE_Y * GRID_SIZE_Z);
    }
    ~SPHSolver() = default;

    void update(float dt);
    void reset();
    void spawnParticles();
    void spawnRandom();

    void resetRestDensity() {
        restDensity = poly6_kernel(0.0f, smoothingRadius);
    }

    // Helper functions to get container bounds
    glm::vec3 getContainerMin() const {
        return containerPosition - containerScale * 0.5f;
    }
    
    glm::vec3 getContainerMax() const {
        return containerPosition + containerScale * 0.5f;
    }

private:
    void computePredictedPositions(float dt);
    void computeGrid();
    void eulerIntegration(float dt);
    void computeDensity();
    void computePressure();
    void computeForces();
    void resolveCollisions();
    void resolveWallCollisions();

    float poly6_kernel(float r, float h) {
        if (r >= h || r < 0.0f) return 0.0f;
        float factor = 315.0f / (64.0f * PI * pow(h, 9));
        return factor * pow(h * h - r * r, 3);
    }

    glm::vec3 poly6_gradient(glm::vec3 r_ij, float r, float h) {
        if (r >= h || r < 0.0f) return glm::vec3(0.0f);
        float coeff = -945.0f / (32.0f * PI * pow(h, 9));
        float factor = pow(h * h - r * r, 2);
        return coeff * factor * (r_ij / r);
    }

    float spikey_kernel(float r, float h) {
        if (r >= h || r < 0.0f) return 0.0f;
        float factor = 15.0f / (PI * pow(h, 6));
        return factor * pow(h - r, 3);
    }

    glm::vec3 spikey_gradient(glm::vec3 r_ij, float r, float h) {
        if (r >= h || r < 0.0f) return glm::vec3(0.0f);
        float coeff = -45.0f / (PI * pow(h, 6));
        float factor = (h - r) * (h - r);
        return coeff * factor * (r_ij / r);
    }

    uint32_t getParticleIndex(const glm::vec3& position) const {
        glm::vec3 translation = GRID_CENTER - glm::vec3(GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z) * 0.5f;
        glm::vec3 relativePos = position - translation;

        int gridX = static_cast<int>(relativePos.x);
        int gridY = static_cast<int>(relativePos.y);
        int gridZ = static_cast<int>(relativePos.z);

        gridX = std::clamp(gridX, 0, static_cast<int>(GRID_SIZE_X) - 1);
        gridY = std::clamp(gridY, 0, static_cast<int>(GRID_SIZE_Y) - 1);
        gridZ = std::clamp(gridZ, 0, static_cast<int>(GRID_SIZE_Z) - 1);

        return flatIndexFromGrid(
            static_cast<uint32_t>(gridX),
            static_cast<uint32_t>(gridY),
            static_cast<uint32_t>(gridZ)
        );
    }

    uint32_t flatIndexFromGrid(uint32_t x, uint32_t y, uint32_t z) const {
        return x + y * GRID_SIZE_X + z * GRID_SIZE_X * GRID_SIZE_Y;
    }

    glm::uvec3 gridIndexFromFlat(uint32_t flatIndex) const {
        uint32_t z = flatIndex / (GRID_SIZE_X * GRID_SIZE_Y);
        uint32_t y = (flatIndex % (GRID_SIZE_X * GRID_SIZE_Y)) / GRID_SIZE_X;
        uint32_t x = flatIndex % GRID_SIZE_X;
        return glm::uvec3(x, y, z);
    }

    bool isValidGridIndex(int32_t x, int32_t y, int32_t z) const {
        return (x < GRID_SIZE_X) && (x >= 0) &&
               (y < GRID_SIZE_Y) && (y >= 0) &&
               (z < GRID_SIZE_Z) && (z >= 0);
    }

    std::array<uint32_t, 27> getNeighbourIndices(uint32_t x, uint32_t y, uint32_t z) const {
        std::array<uint32_t, 27> indices;
        int index = 0;
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int32_t nx = x + dx;
                    int32_t ny = y + dy;
                    int32_t nz = z + dz;
                    if (!isValidGridIndex(nx, ny, nz)) {
                        indices[index++] = UINT32_MAX;
                        continue;
                    }
                    indices[index++] = flatIndexFromGrid(nx, ny, nz);
                }
            }
        }
        return indices;
    }
};

#endif // SPH_SOLVER_HPP