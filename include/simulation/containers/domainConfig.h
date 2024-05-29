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
    int subsampling;

    size_t numParticles;
    float radius;
} Config;
