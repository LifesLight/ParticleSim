#include "simulation/forces/collision.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void handleCollision(Particle* a, Particle* b, float friction) {
    float dx = a->pos[0] - b->pos[0];
    float dy = a->pos[1] - b->pos[1];
    float dz = a->pos[2] - b->pos[2];

    float distance = sqrtf(dx * dx + dy * dy + dz * dz);

    if (distance < a->radius + b->radius) {
        // Normal vector
        float nx = dx / distance;
        float ny = dy / distance;
        float nz = dz / distance;

        // Relative velocity
        float vx = a->vel[0] - b->vel[0];
        float vy = a->vel[1] - b->vel[1];
        float vz = a->vel[2] - b->vel[2];

        // Dot product of relative velocity and normal vector
        float vDotN = vx * nx + vy * ny + vz * nz;

        // Only resolve if particles are moving towards each other
        if (vDotN > 0) return;

        // Collision response
        float impulse = vDotN * friction;

        a->vel[0] -= impulse * nx;
        a->vel[1] -= impulse * ny;
        a->vel[2] -= impulse * nz;

        b->vel[0] += impulse * nx;
        b->vel[1] += impulse * ny;
        b->vel[2] += impulse * nz;
    }
}

void checkCollision(Domain *domain) {
    const size_t particles = domain->config.numParticles;

    // Check for collisions
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        const int chunkX = particle->pos[0] / domain->chunkSize;
        const int chunkY = particle->pos[1] / domain->chunkSize;
        const int chunkZ = particle->pos[2] / domain->chunkSize;

        Chunk *chunk = &domain->chunks[chunkX][chunkY][chunkZ];
        const int chunkParticles = chunk->numParticles;

        // Check for this particle in the chunk
        for (int j = 0; j < chunkParticles; ++j) {
            Particle *other = chunk->particles[j];

            if (other == particle) continue;

            handleCollision(particle, other, domain->config.friction);
        }

        // Check for particles in adjacent chunks
        for (int j = 0; j < 6; ++j) {
            Chunk *adj = chunk->adj[j];

            if (adj == NULL) continue;

            const int adjParticles = adj->numParticles;

            for (int k = 0; k < adjParticles; ++k) {
                Particle *other = adj->particles[k];

                handleCollision(particle, other, domain->config.friction);
            }
        }
    }
}
