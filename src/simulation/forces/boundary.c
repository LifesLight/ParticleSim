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
    if (newPosX - particle->mass < 0) {
        particle->vel.x *= -friction;
        particle->pos.x = particle->mass;
    } else if (newPosX + particle->mass >= DIM_X) {
        particle->vel.x *= -friction;
        particle->pos.x = DIM_X - particle->mass;
    }

    if (newPosY - particle->mass < 0) {
        particle->vel.y *= -friction;
        particle->pos.y = particle->mass;
    } else if (newPosY + particle->mass >= DIM_Y) {
        particle->vel.y *= -friction;
        particle->pos.y = DIM_Y - particle->mass;
    }

    if (newPosZ - particle->mass < 0) {
        particle->vel.z *= -friction;
        particle->pos.z = particle->mass;
    } else if (newPosZ + particle->mass >= DIM_Z) {
        particle->vel.z *= -friction;
        particle->pos.z = DIM_Z - particle->mass;
    }
}
