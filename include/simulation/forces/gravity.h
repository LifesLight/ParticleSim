#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */


#include "simulation/containers/particle.h"
#include "simulation/containers/domain.h"
#include "simulation/containers/domainConfig.h"

void applyGravity(Particle* particle, Domain *domain);
