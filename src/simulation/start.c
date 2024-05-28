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
    const int DIM_X = domain->DIM_X;
    const int DIM_Y = domain->DIM_Y;
    const int DIM_Z = domain->DIM_Z;

    const size_t particles = domain->numParticles;

    for (int i = 0; i < particles; ++i) {
        Particle *particle = &domain->particles[i];

        // Update the particle
        particle->pos[0] += particle->vel[0];
        particle->pos[1] += particle->vel[1];
        particle->pos[2] += particle->vel[2];
    }
}

void startSimulation(Domain* visualizerDomain) {
    Domain domain;

    initDomain(&domain, 50, 50, 50, 100);

    // spawn particles
    srand(time(NULL));
    for (int i = 0; i < domain.numParticles; ++i) {
        Particle *particle = &domain.particles[i];

        particle->pos[0] = rand() % domain.DIM_X;
        particle->pos[1] = rand() % domain.DIM_Y;
        particle->pos[2] = rand() % domain.DIM_Z;

        particle->vel[0] = (rand() % 100) / 100.0f;
        particle->vel[1] = (rand() % 100) / 100.0f;
        particle->vel[2] = (rand() % 100) / 100.0f;
    }

    while(true) {
        stepGlobal(&domain);
        updateDraw(&domain, visualizerDomain);
        usleep(100000);
    }
}
