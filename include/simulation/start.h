#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/containers/domain.h"
#include "simulation/containers/cell.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

void startSimulation(Domain* domain);

#ifdef __cplusplus
}
#endif
