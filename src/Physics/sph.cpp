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
    for (float x = -1.0f; x <= 1.0f; x += radius) {
        for (float y = 1.0f; y <= 1.0f; y += radius) {
            for (float z = -1.0f; z <= 1.0f; z += radius) {
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
                if (delta == glm::vec3(0.0f)) {
                    normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }
                float overlap = radius * 2.0f - distance;
                particles[i].position += normal * overlap * 0.5f;
                particles[j].position -= normal * overlap * 0.5f;

                // Simple velocity response

                float restitution = 0.5f;  // Bounciness [0 = inelastic, 1 = fully elastic]
                float damping = 0.1f;      // Extra damping factor

                glm::vec3 relativeVelocity = particles[i].velocity - particles[j].velocity;
                float relVelAlongNormal = glm::dot(relativeVelocity, normal);

                // Only resolve if they are moving toward each other
                if (relVelAlongNormal < 0.0f) {
                    float impulse = -(1.0f + restitution) * relVelAlongNormal;
                    impulse *= 0.5f; // Split equally (or adjust based on mass if needed)

                    glm::vec3 impulseVec = impulse * normal;

                    particles[i].velocity += impulseVec * (1.0f - damping);
                    particles[j].velocity -= impulseVec * (1.0f - damping);
                }
                
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