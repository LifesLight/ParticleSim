#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/containers/domain.h"
#include "simulation/containers/particle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

void startSimulation(Domain* domain, int numParticles, int size_x, int size_y, int size_z);

#ifdef __cplusplus
}
#endif
