#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/containers/domain.h"
#include "simulation/containers/particle.h"

#include "simulation/forces/boundary.h"
#include "simulation/forces/gravity.h"
#include "simulation/forces/collision.h"
#include "simulation/forces/pressure.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

void startSimulation(Domain* domain, Config config);

#ifdef __cplusplus
}
#endif
