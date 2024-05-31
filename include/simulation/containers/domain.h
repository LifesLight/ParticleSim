#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/containers/particle.h"
#include "simulation/containers/domainConfig.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef struct Domain Domain;

#include "simulation/containers/chunk.h"

struct Domain {
    bool drawable;
    Particle *particles;

    float chunkSize;
    int chunkCounts[3];
    Chunk ***chunks;

    Config config;
};


void initDomain(Domain* domain, Config config);
