#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/math/vector3.h"

#include <stdlib.h>

typedef struct {
    int dim[3];

    float friction;
    V3 gravity;
    float repulsion;

    float speed;
    int supsampling;
    int fps;

    size_t numParticles;
    float mass;

    int targetChunkCount;

    float __internalSpeedFactor;
} Config;
