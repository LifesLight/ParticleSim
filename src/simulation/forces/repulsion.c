#include "simulation/forces/repulsion.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void handleRepulsion(Particle *a, Particle *b, float distance, float repulsion) {
    float overlap = a->mass + b->mass - distance;

    if (overlap > 0) {
        float force = overlap * repulsion;

        V3 dif = sub3(&a->pos, &b->pos);

        float norm = len3(&dif);

        const V3 normal = div3(&dif, norm);

        const V3 forceScaled = mul3(&normal, force);

        a->vel = add3(&a->vel, &forceScaled);
        b->vel = sub3(&b->vel, &forceScaled);
    }
}
