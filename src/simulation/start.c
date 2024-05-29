#include "simulation/start.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#define SUBSAMPLING 1
#define TARGET_FPS 30
#define SPEED 0.1f

#define GRAVITY 0.01f
#define FRICTION 0.98f

// #define REMDIM

void updateDraw(Domain *source, Domain *target) {
    // Update the visualizer domain
    source->drawable = true;
    memccpy(target, source, sizeof(Domain), sizeof(Domain));
    source->drawable = false;
}


void applyExternalForces(Particle *particle) {
    // Apply gravity
    particle->vel[1] -= (GRAVITY / SUBSAMPLING) * SPEED;
}

void handleCollision(Particle* a, Particle* b) {
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
        float impulse = vDotN * FRICTION;

        a->vel[0] -= impulse * nx;
        a->vel[1] -= impulse * ny;
        a->vel[2] -= impulse * nz;

        b->vel[0] += impulse * nx;
        b->vel[1] += impulse * ny;
        b->vel[2] += impulse * nz;
    }
}

void stepGlobal(Domain *domain) {
    const int DIM_X = domain->DIM_X;
    const int DIM_Y = domain->DIM_Y;
    const int DIM_Z = domain->DIM_Z;

    const size_t particles = domain->numParticles;

    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        applyExternalForces(particle);
    }

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

            handleCollision(particle, other);
        }

        // Check for particles in adjacent chunks
        for (int j = 0; j < 6; ++j) {
            Chunk *adj = chunk->adj[j];

            if (adj == NULL) continue;

            const int adjParticles = adj->numParticles;

            for (int k = 0; k < adjParticles; ++k) {
                Particle *other = adj->particles[k];

                handleCollision(particle, other);
            }
        }
    }

    // Update particle positions and handle boundary collisions
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        particle->pos[0] += particle->vel[0];
        particle->pos[1] += particle->vel[1];
        particle->pos[2] += particle->vel[2];

        // Check if we would go out of bounds
        if (particle->pos[0] - particle->radius < 0 || particle->pos[0] + particle->radius >= DIM_X) {
            particle->vel[0] *= -1;
            if (particle->pos[0] - particle->radius < 0) particle->pos[0] = particle->radius;
            if (particle->pos[0] + particle->radius >= DIM_X) particle->pos[0] = DIM_X - particle->radius;
        }

        if (particle->pos[1] - particle->radius < 0 || particle->pos[1] + particle->radius >= DIM_Y) {
            particle->vel[1] *= -1;
            if (particle->pos[1] - particle->radius < 0) particle->pos[1] = particle->radius;
            if (particle->pos[1] + particle->radius >= DIM_Y) particle->pos[1] = DIM_Y - particle->radius;
        }

        if (particle->pos[2] - particle->radius < 0 || particle->pos[2] + particle->radius >= DIM_Z) {
            particle->vel[2] *= -1;
            if (particle->pos[2] - particle->radius < 0) particle->pos[2] = particle->radius;
            if (particle->pos[2] + particle->radius >= DIM_Z) particle->pos[2] = DIM_Z - particle->radius;
        }
    }
}


void startSimulation(Domain* visualizerDomain, int numParticles, int size_x, int size_y, int size_z, float radius) {
    Domain domain;

    initDomain(&domain, size_x, size_y, size_z, numParticles);

    // spawn particles
    srand(time(NULL));

    float maxInitialVelocity = (1000.0f * SUBSAMPLING) / SPEED;

    for (int i = 0; i < domain.numParticles; ++i) {
        Particle *particle = &domain.particles[i];

        particle->pos[0] = (float)rand() / RAND_MAX * domain.DIM_X;
        particle->pos[1] = (float)rand() / RAND_MAX * domain.DIM_Y;
        particle->pos[2] = (float)rand() / RAND_MAX * domain.DIM_Z;

        #ifdef REMDIM
        particle->pos[2] = 0;
        #endif

        particle->vel[0] = (rand() % 100) / maxInitialVelocity;
        particle->vel[1] = (rand() % 100) / maxInitialVelocity;
        particle->vel[2] = (rand() % 100) / maxInitialVelocity;

        #ifdef REMDIM
        particle->vel[2] = 0.0f;
        #endif

        particle->col[0] = rand() % 255;
        particle->col[1] = rand() % 255;
        particle->col[2] = rand() % 255;

        particle->radius = radius;
    }

    const double frameDuration = 1.0 / TARGET_FPS;

    struct timespec start, end;
    double elapsedTime;
    double totalTime = 0.0;
    int frameCount = 0;
    bool runningSlow = false;

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Perform the simulation steps for subsampling
        for (int i = 0; i < SUBSAMPLING; ++i) {
            updateChunks(&domain);
            stepGlobal(&domain);
        }

        // Update the visualization with the new state
        updateDraw(&domain, visualizerDomain);

        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        totalTime += elapsedTime;
        frameCount++;

        if (elapsedTime > frameDuration) {
            runningSlow = true;
        }

        if (frameCount % TARGET_FPS == 0) {
            double averageTime = totalTime / frameCount;
            printf("Average time per frame: %.6f seconds", averageTime);

            if (runningSlow) {
                printf(" Warning: Running slow");
                runningSlow = false;
            }

            printf("\n");
        }

        double sleepTime = frameDuration - elapsedTime;
        if (sleepTime > 0) {
            struct timespec sleepDuration;
            sleepDuration.tv_sec = (time_t)sleepTime;
            sleepDuration.tv_nsec = (sleepTime - sleepDuration.tv_sec) * 1e9;
            nanosleep(&sleepDuration, NULL);
        }
    }
}
