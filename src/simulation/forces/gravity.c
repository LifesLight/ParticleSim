#include "simulation/forces/gravity.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void applyGravity(Particle* particle, Domain *domain) {
    particle->vel[1] -= domain->config.gravity * domain->config.__internalSpeedFactor;
}
