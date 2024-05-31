#include "simulation/math/vector3.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

V3 add3(const V3 *a, const V3 *b) {
    return (V3) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z
    };
}

V3 sub3(const V3 *a, const V3 *b) {
    return (V3) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };
}

V3 mul3(const V3 *a, float b) {
    return (V3) {
        .x = a->x * b,
        .y = a->y * b,
        .z = a->z * b
    };
}

V3 div3(const V3 *a, float b) {
    return (V3) {
        .x = a->x / b,
        .y = a->y / b,
        .z = a->z / b
    };
}

float dot3(const V3 *a, const V3 *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

V3 cross3(const V3 *a, const V3 *b) {
    return (V3) {
        .x = a->y * b->z - a->z * b->y,
        .y = a->z * b->x - a->x * b->z,
        .z = a->x * b->y - a->y * b->x
    };
}

float len3(const V3 *a) {
    return sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
}
