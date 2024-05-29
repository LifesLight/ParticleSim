#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float pos[3];
    float vel[3];
    uint8_t col[3];
    float radius;
} Particle;