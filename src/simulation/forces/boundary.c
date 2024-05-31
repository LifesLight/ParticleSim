#include "simulation/forces/boundary.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void checkBoundaries(Particle *particle, Domain *domain) {
    // Predict the new position
    const V3 newPos = add3(&particle->pos, &particle->vel);

    const float newPosX = newPos.x;
    const float newPosY = newPos.y;
    const float newPosZ = newPos.z;

    const int DIM_X = domain->config.dim[0];
    const int DIM_Y = domain->config.dim[1];
    const int DIM_Z = domain->config.dim[2];

    const float friction = domain->config.friction;

    // Check if we would go out of bounds and correct position and velocity if necessary
    if (newPosX - particle->radius < 0) {
        particle->vel.x *= -friction;
        particle->pos.x = particle->radius;
    } else if (newPosX + particle->radius >= DIM_X) {
        particle->vel.x *= -friction;
        particle->pos.x = DIM_X - particle->radius;
    }

    if (newPosY - particle->radius < 0) {
        particle->vel.y *= -friction;
        particle->pos.y = particle->radius;
    } else if (newPosY + particle->radius >= DIM_Y) {
        particle->vel.y *= -friction;
        particle->pos.y = DIM_Y - particle->radius;
    }

    if (newPosZ - particle->radius < 0) {
        particle->vel.z *= -friction;
        particle->pos.z = particle->radius;
    } else if (newPosZ + particle->radius >= DIM_Z) {
        particle->vel.z *= -friction;
        particle->pos.z = DIM_Z - particle->radius;
    }
}
