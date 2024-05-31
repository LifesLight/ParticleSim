#include "simulation/forces/gravity.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void applyGravity(Particle* particle, V3 *gravity) {
    particle->vel = add3(&particle->vel, gravity);
}
