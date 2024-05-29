#include "simulation/start.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#define TARGET_FPS 30

// #define REMDIM

void updateDraw(Domain *source, Domain *target) {
    // Update the visualizer domain
    source->drawable = true;
    memccpy(target, source, sizeof(Domain), sizeof(Domain));
    source->drawable = false;
}

void stepGlobal(Domain *domain) {
    const size_t particles = domain->config.numParticles;

    // Apply forces
    checkCollision(domain);

    // Global applies
    for (int i = 0; i < particles; i++) {
        Particle *particle = &domain->particles[i];

        applyGravity(particle, domain);
        checkBoundaries(particle, domain);
    }

    // Update positions
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        particle->pos[0] += particle->vel[0];
        particle->pos[1] += particle->vel[1];
        particle->pos[2] += particle->vel[2];
    }
}


void startSimulation(Domain* visualizerDomain, Config config) {
    Domain domain;

    initDomain(&domain, config);

    // spawn particles
    srand(time(NULL));

    float maxInitialVelocity = (1000.0f * config.subsampling) / config.speed;

    for (int i = 0; i < domain.config.numParticles; ++i) {
        Particle *particle = &domain.particles[i];

        particle->pos[0] = (float)rand() / RAND_MAX * (domain.config.dim[0] - 0.2) + 0.1;
        particle->pos[1] = (float)rand() / RAND_MAX * (domain.config.dim[1] - 0.2) + 0.1;
        particle->pos[2] = (float)rand() / RAND_MAX * (domain.config.dim[2] - 0.2) + 0.1;

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

        particle->radius = config.radius;
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
        for (int i = 0; i < config.subsampling; ++i) {
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
