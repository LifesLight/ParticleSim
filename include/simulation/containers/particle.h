#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include <stdbool.h>

typedef struct {
    float pos[3];
    float vel[3];
    float col[3];
    float radius;
} Particle;