#include "simulation/containers/domain.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void initDomain(Domain* domain, int x, int y, int z, int numParticles) {
    domain->DIM_X = x;
    domain->DIM_Y = y;
    domain->DIM_Z = z;
    domain->numParticles = numParticles;

    Particle defaultParticle;
    defaultParticle.pos[0] = 0.0f;
    defaultParticle.pos[1] = 0.0f;
    defaultParticle.pos[2] = 0.0f;

    defaultParticle.vel[0] = 0.0f;
    defaultParticle.vel[1] = 0.0f;
    defaultParticle.vel[2] = 0.0f;

    defaultParticle.radius = 50.0f;

    // Allocate memory for the particles
    domain->particles = (Particle*)malloc(numParticles * sizeof(Particle));
}
