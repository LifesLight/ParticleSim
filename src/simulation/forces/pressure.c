#include "simulation/forces/pressure.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#define SMOOTHING_LENGTH 10.0f

float smoothingKernel(float dist, float h) {
    const float q = dist / h;

    if (q >= 0 && q < 1) {
        return 1 - 1.5f * q * q + 0.75f * q * q * q;
    } else if (q >= 1 && q < 2) {
        return 0.25f * (2 - q) * (2 - q) * (2 - q);
    } else {
        return 0;
    }
}

void updateDensity(Particle *a, Particle *b, float h) {
    const V3 diff = sub3(&a->pos, &b->pos);
    const float dist = len3(&diff);

    a->density += b->mass * smoothingKernel(dist, h);
}

void handlePressure(Domain *domain) {
    const int particles = domain->config.numParticles;
    const float h = SMOOTHING_LENGTH;

    // Compute density for all particles
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];
        particle->density = 0;

        const V3 temp = div3(&particle->pos, domain->chunkSize);

        const int chunkX = temp.x;
        const int chunkY = temp.y;
        const int chunkZ = temp.z;

        Chunk *chunk = &domain->chunks[chunkX][chunkY][chunkZ];
        const int chunkParticles = chunk->numParticles;

        // Check for this particle in the chunk
        for (int j = 0; j < chunkParticles; ++j) {
            Particle *other = chunk->particles[j];
            if (other == particle) continue;

            updateDensity(particle, other, h);
        }

        // Check for particles in adjacent chunks
        for (int j = 0; j < 26; ++j) {
            Chunk *adj = chunk->adj[j];
            if (adj == NULL) continue;

            const int adjParticles = adj->numParticles;
            for (int k = 0; k < adjParticles; ++k) {
                Particle *other = adj->particles[k];
                updateDensity(particle, other, h);
            }
        }

        // Add self-contribution to density
        particle->density += particle->mass * smoothingKernel(0, h);
    }

    // Compute pressure for all particles
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];
        const float density0 = 1.0f; // reference density for water
        const float stiffness = 1000.0f; // stiffness coefficient for pressure force
        const float pressure = stiffness * (particle->density - density0);
        particle->pressure = pressure;
    }

    // Compute pressure forces
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];
        V3 pressureForce = {0, 0, 0};

        const V3 temp = div3(&particle->pos, domain->chunkSize);

        const int chunkX = temp.x;
        const int chunkY = temp.y;
        const int chunkZ = temp.z;

        Chunk *chunk = &domain->chunks[chunkX][chunkY][chunkZ];
        const int chunkParticles = chunk->numParticles;

        // Check for this particle in the chunk
        for (int j = 0; j < chunkParticles; ++j) {
            Particle *other = chunk->particles[j];
            if (other == particle) continue;

            const V3 diff = sub3(&particle->pos, &other->pos);
            const float dist = len3(&diff);

            const float q = dist / h;
            if (q >= 0 && q < 1) {
                const float pressure = 0.5f * (particle->pressure + other->pressure) * (1 - q);
                const V3 kernel = mul3(&diff, 1 / dist * smoothingKernel(dist, h));
                const V3 temp = mul3(&kernel, -pressure);
                pressureForce = add3(&pressureForce, &temp);
            }
        }

        // Check for particles in adjacent chunks
        for (int j = 0; j < 26; ++j) {
            Chunk *adj = chunk->adj[j];
            if (adj == NULL) continue;

            const int adjParticles = adj->numParticles;
            for (int k = 0; k < adjParticles; ++k) {
                Particle *other = adj->particles[k];

                const V3 diff = sub3(&particle->pos, &other->pos);
                const float dist = len3(&diff);

                const float q = dist / h;
                if (q >= 0 && q < 1) {
                    const float pressure = 0.5f * (particle->pressure + other->pressure) * (1 - q);
                    const V3 kernel = mul3(&diff, 1 / dist * smoothingKernel(dist, h));
                    const V3 temp = mul3(&kernel, -pressure);
                    pressureForce = add3(&pressureForce, &temp);
                }
            }
        }

        // Apply pressure force
        particle->vel = add3(&particle->vel, &pressureForce);
    }
}

