#include "simulation/forces/gravity.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void applyGravity(Particle* particle, Domain *domain) {
    const float gravity = domain->config.gravity;
    const float speed = domain->config.speed;
    const float subsampling = domain->config.subsampling;
    particle->vel[1] -= (gravity / subsampling) * speed;
}
