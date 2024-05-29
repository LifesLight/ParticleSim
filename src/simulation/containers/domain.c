#include "simulation/containers/domain.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void initDomain(Domain* domain, int x, int y, int z, int numParticles) {
    domain->DIM_X = x;
    domain->DIM_Y = y;
    domain->DIM_Z = z;
    domain->numParticles = numParticles;

    // Allocate memory for the particles
    domain->particles = (Particle*)malloc(numParticles * sizeof(Particle));
}
