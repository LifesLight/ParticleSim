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

// Predeclare Chunk
typedef struct Chunk Chunk;

struct Chunk {
    Chunk *adj[26];

    int numParticles;
    int size;
    Particle **particles;
};

typedef struct {
    bool drawable;
    Particle *particles;

    float chunkSize;
    int chunkCounts[3];
    Chunk ***chunks;

    Config config;
} Domain;

void initDomain(Domain* domain, Config config);

void updateChunks(Domain* domain);
