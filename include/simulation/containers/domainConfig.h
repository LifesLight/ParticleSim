#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include <stdlib.h>

typedef struct {
    int dim[3];

    float friction;
    float gravity;
    float repulsion;

    float speed;
    int supsampling;
    int fps;

    size_t numParticles;
    float radius;

    int targetChunkCount;

    float __internalSpeedDividor;
} Config;
