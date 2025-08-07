#include "sph.hpp"

#include <iostream>
#include <random>

void SPHSolver::computePredictedPositions(float dt) {
    for (size_t i = 0; i < particles.size(); ++i) {
        Particle& particle = particles[i];
        // Predict position using current velocity
        predictedPositions[i] = particle.position + particle.velocity * dt;
        
        // Apply boundary constraints to predicted positions
        glm::vec3 containerMin = getContainerMin();
        glm::vec3 containerMax = getContainerMax();
        
        predictedPositions[i].x = std::clamp(predictedPositions[i].x, 
                                                 containerMin.x + radius, 
                                                 containerMax.x - radius);
        predictedPositions[i].y = std::clamp(predictedPositions[i].y, 
                                                 containerMin.y + radius, 
                                                 containerMax.y - radius);
        predictedPositions[i].z = std::clamp(predictedPositions[i].z, 
                                                 containerMin.z + radius, 
                                                 containerMax.z - radius);
    }
}

void SPHSolver::computeGrid() {
    for (auto& cell : grid) {
        cell.clear();
    }
    
    for (size_t particleIdx = 0; particleIdx < particles.size(); ++particleIdx) {
        uint32_t cellIdx = getParticleIndex(predictedPositions[particleIdx]);
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

void SPHSolver::computeDensity() {
    float h = smoothingRadius;
    for (size_t particleIdx = 0; particleIdx < particles.size(); ++particleIdx) {
        float density = 0.0f;
        const Particle& particle = particles[particleIdx];
        uint32_t cellIdx = getParticleIndex(predictedPositions[particleIdx]);
        const auto& neighs = grid[cellIdx];
        for (uint32_t neighbourIdx : neighs) {
            const Particle& neighbour = particles[neighbourIdx];
            float r = glm::length(predictedPositions[particleIdx] - predictedPositions[neighbourIdx]);
            if (r >= h) continue;
            density += spikey_kernel(r, h);
        }
        densities[particleIdx] = density;
    }
}

void SPHSolver::computePressure() {
    for (size_t i = 0; i < particles.size(); ++i) {
        float pressure = GAS_CONSTANT * (densities[i] - restDensity);
        if (pressure > 0) {
            pressures[i] = pressure;
        } else {
            pressures[i] = 0;
        }
    }
}

void SPHSolver::computeForces() {
    float h = smoothingRadius;
    for (size_t particleIdx = 0; particleIdx < particles.size(); ++particleIdx) {
        glm::vec3 pressureForce(0.0f);

        const Particle& particle = particles[particleIdx];
        float pi_density = densities[particleIdx];
        float pi_pressure = pressures[particleIdx];

        uint32_t cellIdx = getParticleIndex(predictedPositions[particleIdx]);
        const auto& neighs = grid[cellIdx];
        for (uint32_t neighbourIdx : neighs) {
            if (neighbourIdx == particleIdx) continue;
            const Particle& neighbour = particles[neighbourIdx];
            float pj_density = densities[neighbourIdx];
            float pj_pressure = pressures[neighbourIdx];

            glm::vec3 r_ij = predictedPositions[particleIdx] - predictedPositions[neighbourIdx];
            float r = glm::length(r_ij);
            if (r >= h || r == 0.0f) continue;

            glm::vec3 gradW = spikey_gradient(r_ij, r, h);
            pressureForce += -gradW * (pi_pressure + pj_pressure) / (2.0f * pj_density);
        }
        glm::vec3 gravity(0.0f, gravity_m, 0.0f);
        if (densities[particleIdx] != 0.0f) {
            forces[particleIdx] = (pressureForce + gravity) / densities[particleIdx];
        } else {
            forces[particleIdx] = glm::vec3(0.0f);
        }
    }
}

void SPHSolver::update(float dt) {
    if (particles.empty()) return;
    computePredictedPositions(dt);
    computeGrid();
    computeDensity();
    computePressure();
    computeForces();
    eulerIntegration(dt);

    //resolveCollisions();
    resolveWallCollisions();
}

void SPHSolver::eulerIntegration(float dt) {
    for (size_t i = 0; i < particles.size(); ++i) {
        Particle& particle = particles[i];
        
        particle.velocity += forces[i] * dt;
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
                
                particle.velocity = glm::vec3(0.0f, 0.0f, 0.0f); // Initial velocity
                particles.push_back(particle);
            }
        }
    }
    // fill densities with initial values
    predictedPositions.resize(particles.size(), glm::vec3(0.0f));
    densities.resize(particles.size(), 0.0f);
    pressures.resize(particles.size(), 0.0f);
    forces.resize(particles.size(), glm::vec3(0.0f));
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
    predictedPositions.resize(particles.size(), glm::vec3(0.0f));
    densities.resize(particles.size(), 0.0f);
    pressures.resize(particles.size(), 0.0f);
    forces.resize(particles.size(), glm::vec3(0.0f));
}

void SPHSolver::resolveWallCollisions() {
    glm::vec3 containerMin = getContainerMin();
    glm::vec3 containerMax = getContainerMax();
    
    for (auto& particle : particles) {
        // Bottom wall (Y-axis)
        if (particle.position.y < containerMin.y + radius) {
            particle.position.y = containerMin.y + radius;
            particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        // Top wall (Y-axis)
        if (particle.position.y > containerMax.y - radius) {
            particle.position.y = containerMax.y - radius;
            particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        
        // Left wall (X-axis)
        if (particle.position.x < containerMin.x + radius) {
            particle.position.x = containerMin.x + radius;
            particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        // Right wall (X-axis)
        if (particle.position.x > containerMax.x - radius) {
            particle.position.x = containerMax.x - radius;
            particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        
        // Front wall (Z-axis)
        if (particle.position.z < containerMin.z + radius) {
            particle.position.z = containerMin.z + radius;
            particle.velocity.z = -particle.velocity.z * 0.5f;
        }
        // Back wall (Z-axis)
        if (particle.position.z > containerMax.z - radius) {
            particle.position.z = containerMax.z - radius;
            particle.velocity.z = -particle.velocity.z * 0.5f;
        }
    }
}

void SPHSolver::reset() {
    particles.clear();
    densities.clear();
    pressures.clear();
    forces.clear();
    for (auto& cell : grid) {
        cell.clear();
    }
}