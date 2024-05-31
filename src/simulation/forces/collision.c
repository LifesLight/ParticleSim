#include "simulation/forces/collision.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void handleCollision(Particle* a, Particle* b, float distance, float friction) {
    V3 dif = sub3(&a->pos, &b->pos);

    if (distance < a->mass + b->mass) {
        // Normal vector
        V3 normal = div3(&dif, distance);

        // Relative velocity
        V3 relVel = sub3(&a->vel, &b->vel);

        // Dot product of relative velocity and normal vector
        float vDotN = dot3(&relVel, &normal);

        // Only resolve if particles are moving towards each other
        if (vDotN > 0) return;

        // Collision response
        float impulse = vDotN * friction;

        // Apply impulse
        const V3 impulseVec = mul3(&normal, impulse);

        a->vel = sub3(&a->vel, &impulseVec);
        b->vel = add3(&b->vel, &impulseVec);
    }
}
