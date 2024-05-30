#include "simulation/forces/repulsion.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void handleRepulsion(Particle* a, Particle* b, float forceFactor) {
    // Calculate the direction from particle b to particle a
    float dx = a->pos[0] - b->pos[0];
    float dy = a->pos[1] - b->pos[1];
    float dz = a->pos[2] - b->pos[2];

    // Calculate the distance between particles
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);

    // Check overlap
    float overlap = a->radius + b->radius - distance;

    if (overlap > 0) {
        // Calculate the force to apply
        float force = overlap * forceFactor;

        // Normalize the direction vector
        float nx = dx / distance;
        float ny = dy / distance;
        float nz = dz / distance;

        // Apply the force to both particles
        a->vel[0] += force * nx;
        a->vel[1] += force * ny;
        a->vel[2] += force * nz;

        b->vel[0] -= force * nx;
        b->vel[1] -= force * ny;
        b->vel[2] -= force * nz;
    }
}

void applyRepulsion(Domain *domain) {
    const size_t particles = domain->config.numParticles;

    const float repulsion = domain->config.repulsion * domain->config.__internalSpeedFactor;

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

            handleRepulsion(particle, other, repulsion);
        }

        // Check for particles in adjacent chunks
        for (int j = 0; j < 26; ++j) {
            Chunk *adj = chunk->adj[j];

            if (adj == NULL) continue;

            const int adjParticles = adj->numParticles;

            for (int k = 0; k < adjParticles; ++k) {
                Particle *other = adj->particles[k];

                handleRepulsion(particle, other, repulsion);
            }
        }
    }
}
