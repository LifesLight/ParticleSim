#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/containers/cell.h"

#include <stdlib.h>

typedef struct {
    int DIM_X;
    int DIM_Y;
    int DIM_Z;

    bool drawable;

    Cell*** cells;
} Domain;

void initDomain(Domain* domain, int size_x, int size_y, int size_z);
