#include "simulation/start.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#define SUBSAMPLING 1
#define TARGET_FPS 60

void updateDraw(Domain *source, Domain *target) {
    // Update the visualizer domain
    source->drawable = true;
    memccpy(target, source, sizeof(Domain), sizeof(Domain));
    source->drawable = false;
}

void stepGlobal(Domain *domain) {
    const int DIM_X = domain->DIM_X;
    const int DIM_Y = domain->DIM_Y;
    const int DIM_Z = domain->DIM_Z;

    const size_t particles = domain->numParticles;

    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        // Update the particle

        // Check for collisions

        for (int j = 0; j < particles; ++j) {
            if (i == j) {
                continue;
            }

            Particle *other = &domain->particles[j];

            float dx = particle->pos[0] - other->pos[0];
            float dy = particle->pos[1] - other->pos[1];
            float dz = particle->pos[2] - other->pos[2];

            float distance = sqrt(dx * dx + dy * dy + dz * dz);

            if (distance < particle->radius + other->radius) {
                // Collision
                particle->vel[0] *= -1;
                particle->vel[1] *= -1;
                particle->vel[2] *= -1;

                other->vel[0] *= -1;
                other->vel[1] *= -1;
                other->vel[2] *= -1;
            }
        }
        

        // Check if we would go out of bounds
        if (particle->pos[0] + particle->vel[0] < 0 || particle->pos[0] + particle->vel[0] >= DIM_X) {
            particle->vel[0] *= -1;
        }

        if (particle->pos[1] + particle->vel[1] < 0 || particle->pos[1] + particle->vel[1] >= DIM_Y) {
            particle->vel[1] *= -1;
        }

        if (particle->pos[2] + particle->vel[2] < 0 || particle->pos[2] + particle->vel[2] >= DIM_Z) {
            particle->vel[2] *= -1;
        }

        particle->pos[0] += particle->vel[0];
        particle->pos[1] += particle->vel[1];
        particle->pos[2] += particle->vel[2];
    }
}

void startSimulation(Domain* visualizerDomain, int numParticles, int size_x, int size_y, int size_z) {
    Domain domain;

    initDomain(&domain, size_x, size_y, size_z, numParticles);

    // spawn particles
    srand(time(NULL));

    float slowdown = 2.5f;
    float maxInitialVelocity = 100.0f * SUBSAMPLING * slowdown;

    for (int i = 0; i < domain.numParticles; ++i) {
        Particle *particle = &domain.particles[i];

        particle->pos[0] = rand() % domain.DIM_X;
        particle->pos[1] = rand() % domain.DIM_Y;
        particle->pos[2] = rand() % domain.DIM_Z;

        particle->vel[0] = (rand() % 100) / maxInitialVelocity;
        particle->vel[1] = (rand() % 100) / maxInitialVelocity;
        particle->vel[2] = (rand() % 100) / maxInitialVelocity;
    }

    const double frameDuration = 1.0 / TARGET_FPS;

    struct timespec start, end;
    double elapsedTime;
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Perform the simulation steps for subsampling
        for (int i = 0; i < SUBSAMPLING; ++i) {
            stepGlobal(&domain);
        }

        // Update the visualization with the new state
        updateDraw(&domain, visualizerDomain);

        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        double sleepTime = frameDuration - elapsedTime;
        if (sleepTime > 0) {
            struct timespec sleepDuration;
            sleepDuration.tv_sec = (time_t)sleepTime;
            sleepDuration.tv_nsec = (sleepTime - sleepDuration.tv_sec) * 1e9;
            nanosleep(&sleepDuration, NULL);
        }
    }
}
