#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/math/vector3.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    V3 pos;
    V3 vel;
    uint8_t col[3];
    float radius;
} Particle;