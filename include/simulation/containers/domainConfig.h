#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include <stdlib.h>

typedef struct {
    int dim[3];

    float friction;
    float gravity;
    float speed;
    float repulsion;

    int supsampling;

    size_t numParticles;
    float radius;

    int targetChunkCount;
} Config;
