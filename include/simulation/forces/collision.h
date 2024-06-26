#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */


#include "simulation/containers/particle.h"
#include "simulation/containers/domain.h"
#include "simulation/containers/domainConfig.h"

#include <math.h>

void handleCollision(Particle* a, Particle* b, float distance, float friction);
