#include "sph.hpp"

#include <random>

void SPHSolver::computeDensity() {
    float h = radius;
    for (size_t particleIdx = 0; particleIdx < particles.size(); ++particleIdx) {
        float density = 0.0f;
        const Particle& particle = particles[particleIdx];
        uint32_t cellIdx = getParticleIndex(particle.position);
        const auto& neighs = grid[cellIdx];
        for (uint32_t neighbourIdx : neighs) {
            if (neighbourIdx == particleIdx) continue;
            const Particle& neighbour = particles[neighbourIdx];
            float r = glm::length(particle.position - neighbour.position);
            if (r >= h) continue;
            density += poly6_kernel(r, h);
        }
        densities[particleIdx] = density;
        particles[particleIdx].velocity = glm::vec3(densities[particleIdx], 0.0f, 0.0f);
    }
}
void SPHSolver::update(float dt) {
    //eulerIntegration(dt);
    computeGrid();
    computeDensity();


    //resolveCollisions();
    resolveWallCollisions();
}

void SPHSolver::eulerIntegration(float dt) {
    for (auto& particle : particles) {
        particle.velocity += glm::vec3(0.0f, GRAVITY, 0.0f) * dt;
        particle.position += particle.velocity * dt;
    }
}

void SPHSolver::spawnParticles() {
    
    // Calculate how many particles can fit in each dimension
    glm::vec3 containerMin = getContainerMin();
    glm::vec3 availableSpace = containerScale - glm::vec3(2.0f * radius); // Account for particle radius
    
    int particlesPerDim = 10; // You can adjust this or calculate based on container size
    glm::vec3 spacing = availableSpace / float(particlesPerDim - 1);
    
    for (int i = 0; i < particlesPerDim; ++i) {
        for (int j = 0; j < particlesPerDim; ++j) {
            for (int k = 0; k < particlesPerDim; ++k) {
                Particle particle;
                
                // Position particles within the transformed container
                particle.position = containerMin + glm::vec3(radius) + glm::vec3(
                    i * spacing.x,
                    j * spacing.y,
                    k * spacing.z
                );
                
                particle.velocity = glm::vec3(0.0f);
                particles.push_back(particle);
            }
        }
    }
    // fill densities with initial values
    densities.resize(particles.size(), 0.0f);
}

void SPHSolver::spawnRandom() {
    glm::vec3 containerMin = getContainerMin();
    glm::vec3 containerMax = getContainerMax();

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distX(containerMin.x, containerMax.x);
    std::uniform_real_distribution<float> distY(containerMin.y, containerMax.y);
    std::uniform_real_distribution<float> distZ(containerMin.z, containerMax.z);
    
    for (int i = 0; i < 100; ++i) { // Spawn 100 random particles
        Particle particle;
        particle.position = glm::vec3(
            distX(gen),
            distY(gen),
            distZ(gen)
        );
        particle.velocity = glm::vec3(0.0f);
        particles.push_back(particle);
    }
    // fill densities with initial values
    densities.resize(particles.size(), 0.0f);
}

void SPHSolver::resolveCollisions() {
    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = 0; j < particles.size(); ++j) {
            if (i == j) continue;
            glm::vec3 delta = particles[i].position - particles[j].position;
            float distance = glm::length(delta);
            
            if (distance < radius * 2.0f && distance > 0.001f) {
                glm::vec3 normal = glm::normalize(delta);
                float overlap = radius * 2.0f - distance;
                
                // Position correction
                particles[i].position += normal * overlap * 0.5f;
                particles[j].position -= normal * overlap * 0.5f;

                float vel_i = glm::length(particles[i].velocity);
                float vel_j = glm::length(particles[j].velocity);

                particles[i].velocity = normal * vel_i * 0.5f;
                particles[j].velocity = -normal * vel_j * 0.5f;
            }
        }
    }
}

void SPHSolver::resolveWallCollisions() {
    glm::vec3 containerMin = getContainerMin();
    glm::vec3 containerMax = getContainerMax();
    
    for (auto& particle : particles) {
        // Bottom wall (Y-axis)
        if (particle.position.y < containerMin.y + radius) {
            particle.position.y = containerMin.y + radius;
            //particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        // Top wall (Y-axis)
        if (particle.position.y > containerMax.y - radius) {
            particle.position.y = containerMax.y - radius;
            //particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        
        // Left wall (X-axis)
        if (particle.position.x < containerMin.x + radius) {
            particle.position.x = containerMin.x + radius;
            //particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        // Right wall (X-axis)
        if (particle.position.x > containerMax.x - radius) {
            particle.position.x = containerMax.x - radius;
            //particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        
        // Front wall (Z-axis)
        if (particle.position.z < containerMin.z + radius) {
            particle.position.z = containerMin.z + radius;
            //particle.velocity.z = -particle.velocity.z * 0.5f;
        }
        // Back wall (Z-axis)
        if (particle.position.z > containerMax.z - radius) {
            particle.position.z = containerMax.z - radius;
            //particle.velocity.z = -particle.velocity.z * 0.5f;
        }
    }
}

void SPHSolver::computeGrid() {
    for (auto& cell : grid) {
        cell.clear();
    }
    
    for (size_t particleIdx = 0; particleIdx < particles.size(); ++particleIdx) {
        uint32_t cellIdx = getParticleIndex(particles[particleIdx].position);
        glm::uvec3 cellCoords = gridIndexFromFlat(cellIdx);
        std::array<uint32_t, 27> neighbours = getNeighbourIndices(
            cellCoords.x, cellCoords.y, cellCoords.z
        );
        for (uint32_t neighbourIndex : neighbours) {
            if (neighbourIndex == UINT32_MAX) continue;
            grid[neighbourIndex].push_back(particleIdx);
        }
    }
}

void SPHSolver::reset() {
    particles.clear();
    densities.clear();
    grid.clear();
}