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

void handleParticleInteraction(Particle *a, Particle *b, Domain *domain) {
    const V3 delta = sub3(&a->pos, &b->pos);
    float distance = length3(&delta);

    float repulsion = domain->config.repulsion * domain->config.__internalSpeedFactor;
    float friction = domain->config.friction;

    handleRepulsion(a, b, distance, repulsion);
    handleCollision(a, b, distance, friction);
}

void stepGlobal(Domain *domain) {
    const size_t particles = domain->config.numParticles;

    // Apply forces
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

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

            handleParticleInteraction(particle, other, domain);
        }

        // Check for particles in adjacent chunks
        for (int j = 0; j < 26; ++j) {
            Chunk *adj = chunk->adj[j];

            if (adj == NULL) continue;

            const int adjParticles = adj->numParticles;

            for (int k = 0; k < adjParticles; ++k) {
                Particle *other = adj->particles[k];

                handleParticleInteraction(particle, other, domain);
            }
        }
    }

    // Global applies
    V3 gravity = mul3(&domain->config.gravity, domain->config.__internalSpeedFactor);
    for (int i = 0; i < particles; i++) {
        Particle *particle = &domain->particles[i];

        applyGravity(particle, &gravity);
        checkBoundaries(particle, domain);
    }

    // Update positions
    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        particle->pos = add3(&particle->pos, &particle->vel);
    }
}

#define min(a, b) ((a) < (b) ? (a) : (b))


void startSimulation(Domain* visualizerDomain, Config config) {
    Domain domain;

    initDomain(&domain, config);

    // spawn particles
    srand(time(NULL));

    float maxInitialVelocity = 0.005f * domain.config.__internalSpeedFactor;

    printf("Timestep scaling factor: %f\n", domain.config.__internalSpeedFactor);

    // Calculate the volume of the domain
    int custPointScatX = domain.config.dim[0] / 4;
    int custPointScatY = domain.config.dim[1] / 1.5;
    int custPointScatZ = domain.config.dim[2];

    float volume = (custPointScatX - 0.2) * (custPointScatY - 0.2) * (custPointScatZ - 0.2);

    // Calculate the approximate spacing between particles to achieve even distribution
    float spacing = pow(volume / domain.config.numParticles, 1.0 / 3.0);

    // Calculate the number of particles along each dimension
    int numParticlesX = ceil((custPointScatX - 0.2) / spacing);
    int numParticlesY = ceil((custPointScatY - 0.2) / spacing);
    int numParticlesZ = ceil((custPointScatZ - 0.2) / spacing);

    // Recalculate the actual spacing based on the number of particles
    spacing = min((custPointScatX - 0.2) / numParticlesX, min((custPointScatY - 0.2) / numParticlesY, (custPointScatZ - 0.2) / numParticlesZ));

    // Initialize the particles
    for (int i = 0; i < domain.config.numParticles; ++i) {
        Particle *particle = &domain.particles[i];

        // Calculate the grid indices for each particle
        int xIndex = i % numParticlesX;
        int yIndex = (i / numParticlesX) % numParticlesY;
        int zIndex = i / (numParticlesX * numParticlesY);

        // Assign positions based on the grid indices, with padding added
        particle->pos.x = xIndex * spacing + 0.1;
        particle->pos.y = yIndex * spacing + 0.1;
        particle->pos.z = zIndex * spacing + 0.1;

        // Velocity and color settings remain unchanged
        particle->vel.x = (rand() % 100) * maxInitialVelocity;
        particle->vel.y = 0;//(rand() % 100) * maxInitialVelocity;
        particle->vel.z = (rand() % 100) * maxInitialVelocity;

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
