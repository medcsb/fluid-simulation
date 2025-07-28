#include "sph.hpp"


void SPHSolver::update(float dt) {
    for (auto& particle : particles) {
        particle.position += particle.velocity * dt;
        particle.velocity += glm::vec3(0.0f, GRAVITY, 0.0f) * dt;
    }

    resolveCollisions();
    resolveWallCollisions();
}

std::vector<glm::vec3> SPHSolver::spawnParticles() {
    std::vector<glm::vec3> positions;
    for (float x = -0.5f; x <= 0.5f; x += radius) {
        for (float y = 0.5f; y <= 0.5f; y += radius) {
            for (float z = -0.5f; z <= 0.5f; z += radius) {
                Particle particle;
                particle.position = glm::vec3(x, y + radius, z);
                particle.velocity = glm::vec3(0.0f);
                particles.push_back(particle);
                positions.push_back(particle.position);
            }
        }
    }
    return positions;
}

void SPHSolver::resolveCollisions() {
    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = i + 1; j < particles.size(); ++j) {
            glm::vec3 delta = particles[i].position - particles[j].position;
            float distance = glm::length(delta);
            if (distance < radius * 2.0f) {
                glm::vec3 normal = glm::normalize(delta);
                float overlap = radius * 2.0f - distance;
                particles[i].position += normal * overlap * 0.5f;
                particles[j].position -= normal * overlap * 0.5f;
            }
        }
    }
}

void SPHSolver::resolveWallCollisions() {
    for (auto& particle : particles) {
        if (particle.position.y < radius) {
            particle.position.y = radius;
            particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        if (particle.position.y > 1.0f - radius) {
            particle.position.y = 1.0f - radius;
            particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        if (particle.position.x < -1.0f + radius) {
            particle.position.x = -1.0f + radius;
            particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        if (particle.position.x > 1.0f - radius) {
            particle.position.x = 1.0f - radius;
            particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        if (particle.position.z < -1.0f + radius) {
            particle.position.z = -1.0f + radius;
            particle.velocity.z = -particle.velocity.z * 0.5f;
        }
        if (particle.position.z > 1.0f - radius) {
            particle.position.z = 1.0f - radius;
            particle.velocity.z = -particle.velocity.z * 0.5f;
        }
    }
}