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

    const uint32_t CELLS_PER_METER = 10;
    const float CELL_SIZE = 1.0f / CELLS_PER_METER;

    const uint32_t GRID_SIZE_X = 10 * CELLS_PER_METER;
    const uint32_t GRID_SIZE_Y = 10 * CELLS_PER_METER;
    const uint32_t GRID_SIZE_Z = 10 * CELLS_PER_METER;
    const glm::vec3 GRID_CENTER = glm::vec3(0.0f);

    std::vector<std::vector<uint32_t>> grid;
    std::vector<Particle> particles;
    std::vector<float> densities; 

    // Container transform properties
    glm::vec3 containerPosition = glm::vec3(0.0f);  // Center of the container
    glm::vec3 containerScale = glm::vec3(2.0f);     // Full size of container (not half-extents)

    float radius = 0.02f;

    SPHSolver() {
        grid.resize(GRID_SIZE_X * GRID_SIZE_Y * GRID_SIZE_Z);
    }
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
    void computeGrid();
    void eulerIntegration(float dt);
    void computeDensity();
    void resolveCollisions();
    void resolveWallCollisions();

    float poly6_kernel(float r, float h) {
        if (r >= h || r < 0.0f) return 0.0f;
        const float PI = glm::pi<float>();
        float factor = 315.0f / (64.0f * PI * pow(h, 9));
        return factor * pow(h * h - r * r, 3);
    }

    uint32_t getParticleIndex(const glm::vec3& position) const {
        glm::vec3 translation = GRID_CENTER - glm::vec3(GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z) * 0.5f * CELL_SIZE;
        glm::vec3 relativePos = (position - translation) / CELL_SIZE;

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