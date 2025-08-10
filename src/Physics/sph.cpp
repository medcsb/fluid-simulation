#include "sph.hpp"

#include <iostream>
#include <random>

void SPHSolver::update(float dt) {
    predictePositions(dt);
    builGrid();
    computeDensityPressure();
    computeForces();
    integrate(dt);
}

void SPHSolver::predictePositions(float dt) {
    for (size_t i = 0; i < particles.size(); ++i) {
        predictedPositions[i] = particles[i].position + dt * particles[i].velocity;
    }
}

void SPHSolver::builGrid() {
    grid.clear();
    for (size_t i = 0; i < particles.size(); ++i) {
        GridCoord cell = getCellCord(predictedPositions[i]);
        grid[cell].push_back(i);
    }
}

void SPHSolver::computeDensityPressure() {
    for (size_t i = 0; i < particles.size(); i++) {
        densities[i] = 0.0f;
        auto neighbours = getNeighbours(i);
        for (uint32_t j : neighbours) {
            glm::vec3 r_ij = predictedPositions[i] - predictedPositions[j];
            float r2 = glm::dot(r_ij, r_ij);
            if (r2 < h * h) densities[i] += mass * poly6_kernel(r2);
        }
        pressures[i] = pressure_multiplier * (densities[i] - restDensity);
        if (pressures[i] < 0.0f) pressures[i] = 0.0f;
    }
}

void SPHSolver::computeForces() {
    for (size_t i = 0; i < particles.size(); i++) {
        glm::vec3 fPressure(0.0f);
        glm::vec3 fViscosity(0.0f);
        auto neighbours = getNeighbours(i);
        for (uint32_t j : neighbours) {
            if (i == j) continue;
            glm::vec3 r_ij = predictedPositions[i] - predictedPositions[j];
            float rlen = glm::length(r_ij);
            if (rlen < 1e-4f) {
                // chose a random direction to avoid division by zero
                glm::vec3 randomDir = glm::vec3(0.0f, 1.0f, 0.0f);
                float epsDist = epsilon * h;
                particles[i].position += 0.5f * epsDist * randomDir;
                particles[j].position -= 0.5f * epsDist * randomDir;
            }
            if (rlen < h && rlen > 1e-4f) {
                fPressure += -mass * (pressures[i] + pressures[j]) / (2.0f * densities[j]) *
                             spiky_grad(r_ij, rlen);
                fViscosity += viscosity * mass * (particles[j].velocity - particles[i].velocity) / densities[j] *
                              visc_lap(rlen);
            }
        }
        glm::vec3 fGravity(0.0f, gravity_m * densities[i], 0.0f);
        forces[i] = fPressure + fViscosity + fGravity;
    }
}

void SPHSolver::integrate(float dt) {
    glm::vec3 half = boxSize * 0.5f;
    glm::vec3 minB = boxPos - half;
    glm::vec3 maxB = boxPos + half;

    glm::vec3 wallVelMin = ((boxPos - boxSize * 0.5f) - (prevBoxPos - prevBoxSize * 0.5f)) / dt;
    glm::vec3 wallVelMax = ((boxPos + boxSize * 0.5f) - (prevBoxPos + prevBoxSize * 0.5f)) / dt;

    prevBoxPos = boxPos;
    prevBoxSize = boxSize;

    for (size_t i = 0; i < particles.size(); i++) {
        // euler integration
        particles[i].velocity += dt * (forces[i] / mass);
        float speed = glm::length(particles[i].velocity);
        speed = std::min(speed, max_speed);
        particles[i].velocity = glm::normalize(particles[i].velocity) * speed;
        particles[i].position += dt * particles[i].velocity;

        // Boundary conditions
        for (int axis = 0; axis < 3; ++axis) {
            if (particles[i].position[axis] - radius < minB[axis]) {
                particles[i].position[axis] = minB[axis] + radius;
                float relVel = particles[i].velocity[axis] - wallVelMin[axis] / restDensity;
                particles[i].velocity[axis] = wallVelMin[axis] - relVel * bounce;
            } else if (particles[i].position[axis] + radius > maxB[axis]) {
                particles[i].position[axis] = maxB[axis] - radius;
                float relVel = particles[i].velocity[axis] - wallVelMax[axis] / restDensity;
                particles[i].velocity[axis] = wallVelMax[axis] - relVel * bounce;
            }
        }
    }
}

GridCoord SPHSolver::getCellCord(const glm::vec3& position) const {
    GridCoord cell;
    cell.x = static_cast<int>(std::floor(position.x / h));
    cell.y = static_cast<int>(std::floor(position.y / h));
    cell.z = static_cast<int>(std::floor(position.z / h));
    return cell;
}

std::vector<uint32_t> SPHSolver::getNeighbours(uint32_t idx) const {
    std::vector<uint32_t> result;
    GridCoord cell = getCellCord(particles[idx].position);
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                GridCoord nc = {cell.x + dx, cell.y + dy, cell.z + dz};
                auto it = grid.find(nc);
                if (it != grid.end()) result.insert(result.end(), it->second.begin(), it->second.end());
            }
        }
    }
    return result;
}

float SPHSolver::poly6_kernel(float r2) const {
    float hr2 = h2 - r2;
    if (hr2 < 0.0f) return 0.0f;
    const float k = 315.0f / (64.0f * glm::pi<float>() * std::pow(h, 9));
    return k * hr2 * hr2 * hr2;
}

glm::vec3 SPHSolver::spiky_grad(const glm::vec3& r, float rlen) const {
    float hr = h - rlen;
    if (hr < 0.0f) return glm::vec3(0.0f);
    const float k = -45.0f / (glm::pi<float>() * std::pow(h, 6));
    return k * (hr * hr) * (r / rlen);
}

float SPHSolver::visc_lap(float rlen) const {
    if (rlen >= h) return 0.0f;
    const float k = 45.0f / (glm::pi<float>() * std::pow(h, 6));
    return k * (h - rlen);
}

void SPHSolver::spawnParticles() {
    // spawn cube of stacked particles in the box
    glm::vec3 half = boxSize * 0.5f;
    glm::vec3 minB = boxPos - half;

    for (float x = minB.x; x < minB.x + half.x; x += radius * 2.0f) {
        for (float y = minB.y; y < minB.y + half.y; y += radius * 2.0f) {
            for (float z = minB.z; z < minB.z + half.z; z += radius * 2.0f) {
                Particle p;
                p.position = glm::vec3(x, y, z);
                p.velocity = glm::vec3(0.0f);
                particles.push_back(p);
            }
        }
    }
    predictedPositions.resize(particles.size(), glm::vec3(0.0f));
    densities.resize(particles.size(), restDensity);
    pressures.resize(particles.size(), 0.0f);
    forces.resize(particles.size(), glm::vec3(0.0f));
}

void SPHSolver::spawnRandom() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(boxPos.x - boxSize.x / 2, boxPos.x + boxSize.x / 2);
    std::uniform_real_distribution<float> disY(boxPos.y - boxSize.y / 2, boxPos.y + boxSize.y / 2);
    std::uniform_real_distribution<float> disZ(boxPos.z - boxSize.z / 2, boxPos.z + boxSize.z / 2);

    for (size_t i = 0; i < 100; ++i) {
        Particle p;
        p.position = glm::vec3(disX(gen), disY(gen), disZ(gen));
        p.velocity = glm::vec3(0.0f);
        particles.push_back(p);
    }

    predictedPositions.resize(particles.size(), glm::vec3(0.0f));
    densities.resize(particles.size(), restDensity);
    pressures.resize(particles.size(), 0.0f);
    forces.resize(particles.size(), glm::vec3(0.0f));
}

void SPHSolver::reset() {
    particles.clear();
    densities.clear();
    pressures.clear();
    forces.clear();
    grid.clear();
}