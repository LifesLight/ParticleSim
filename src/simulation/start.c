#include "simulation/start.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

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

    applyRepulsion(domain);

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

#define min(a, b) ((a) < (b) ? (a) : (b))


void startSimulation(Domain* visualizerDomain, Config config) {
    Domain domain;

    initDomain(&domain, config);

    // spawn particles
    srand(time(NULL));

    float maxInitialVelocity = 0.01f / domain.config.__internalSpeedDividor;

    // Calculate the volume of the domain
    float volume = (domain.config.dim[0] - 0.2) * (domain.config.dim[1] - 0.2) * (domain.config.dim[2] - 0.2);

    // Calculate the approximate spacing between particles to achieve even distribution
    float spacing = pow(volume / domain.config.numParticles, 1.0 / 3.0);

    // Calculate the number of particles along each dimension
    int numParticlesX = ceil((domain.config.dim[0] - 0.2) / spacing);
    int numParticlesY = ceil((domain.config.dim[1] - 0.2) / spacing);
    int numParticlesZ = ceil((domain.config.dim[2] - 0.2) / spacing);

    // Recalculate the actual spacing based on the number of particles
    spacing = min((domain.config.dim[0] - 0.2) / numParticlesX, min((domain.config.dim[1] - 0.2) / numParticlesY, (domain.config.dim[2] - 0.2) / numParticlesZ));

    for (int i = 0; i < domain.config.numParticles; ++i) {
        Particle *particle = &domain.particles[i];

        // Calculate the grid indices for each particle
        int xIndex = i % numParticlesX;
        int yIndex = (i / numParticlesX) % numParticlesY;
        int zIndex = i / (numParticlesX * numParticlesY);

        // Assign positions based on the grid indices, with padding added
        particle->pos[0] = xIndex * spacing + 0.1;
        particle->pos[1] = yIndex * spacing + 0.1;
        particle->pos[2] = zIndex * spacing + 0.1;

        // Velocity and color settings remain unchanged
        particle->vel[0] = (rand() % 100) * maxInitialVelocity;
        particle->vel[1] = (rand() % 100) * maxInitialVelocity;
        particle->vel[2] = (rand() % 100) * maxInitialVelocity;

        particle->col[0] = rand() % 255;
        particle->col[1] = rand() % 255;
        particle->col[2] = rand() % 255;

        particle->radius = config.radius;
    }


    const double frameDuration = 1.0 / config.fps;

    struct timespec start, end;
    double elapsedTime;
    double totalTime = 0.0;
    int frameCount = 0;
    bool runningSlow = false;

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Perform the simulation steps for supsampling
        for (int i = 0; i < config.supsampling; ++i) {
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

        if (frameCount % config.fps == 0) {
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
