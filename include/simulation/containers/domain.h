#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/containers/particle.h"

#include <stdlib.h>
#include <stdio.h>

// Predeclare Chunk
typedef struct Chunk Chunk;

struct Chunk {
    Chunk *adj[6];

    int numParticles;
    Particle **particles;
};

typedef struct {
    int DIM_X;
    int DIM_Y;
    int DIM_Z;

    bool drawable;
    Particle *particles;
    size_t numParticles;

    float chunkSize;
    int chunkCounts[3];
    Chunk ***chunks;
} Domain;

void initDomain(Domain* domain, int size_x, int size_y, int size_z, int numParticles);

void updateChunks(Domain* domain);
