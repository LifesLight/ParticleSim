#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */


#include "simulation/containers/particle.h"

// Predeclare Chunk
typedef struct Chunk Chunk;

#include "simulation/containers/domain.h"

struct Chunk {
    Chunk *adj[26];

    int numParticles;
    int size;
    Particle **particles;
};


void initChunks(Domain *domain);

void updateChunks(Domain *domain);
