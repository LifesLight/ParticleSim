#include "simulation/containers/domain.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void initDomain(Domain* domain, Config config) {
    config.__internalSpeedFactor = (float) config.speed * ((float) config.fps) / ((float) config.supsampling);
    domain->config = config;

    // Allocate memory for the particles
    domain->particles = (Particle*)malloc(config.numParticles * sizeof(Particle));

    initChunks(domain);
}
