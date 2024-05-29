#include "simulation/forces/boundary.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void checkBoundaries(Particle *particle, Domain *domain) {
    // Predict the new position
    const float newPosX = particle->pos[0] + particle->vel[0];
    const float newPosY = particle->pos[1] + particle->vel[1];
    const float newPosZ = particle->pos[2] + particle->vel[2];

    const int DIM_X = domain->config.dim[0];
    const int DIM_Y = domain->config.dim[1];
    const int DIM_Z = domain->config.dim[2];

    const float friction = domain->config.friction;

    // Check if we would go out of bounds and correct position and velocity if necessary
    if (newPosX - particle->radius < 0) {
        particle->vel[0] *= -friction;
        particle->pos[0] = particle->radius;
    } else if (newPosX + particle->radius >= DIM_X) {
        particle->vel[0] *= -friction;
        particle->pos[0] = DIM_X - particle->radius;
    }

    if (newPosY - particle->radius < 0) {
        particle->vel[1] *= -friction;
        particle->pos[1] = particle->radius;
    } else if (newPosY + particle->radius >= DIM_Y) {
        particle->vel[1] *= -friction;
        particle->pos[1] = DIM_Y - particle->radius;
    }

    if (newPosZ - particle->radius < 0) {
        particle->vel[2] *= -friction;
        particle->pos[2] = particle->radius;
    } else if (newPosZ + particle->radius >= DIM_Z) {
        particle->vel[2] *= -friction;
        particle->pos[2] = DIM_Z - particle->radius;
    }
}
