#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */


#include <math.h>

typedef struct {
    float x;
    float y;
    float z;
} V3;

V3 add3(const V3 *a, const V3 *b);
V3 sub3(const V3 *a, const V3 *b);
V3 mul3(const V3 *a, float b);
V3 div3(const V3 *a, float b);
float dot3(const V3 *a, const V3 *b);
V3 cross3(const V3 *a, const V3 *b);
float len3(const V3 *a);
