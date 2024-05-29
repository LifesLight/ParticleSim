#include "simulation/containers/domain.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void initDomain(Domain* domain, int x, int y, int z, int numParticles) {
    domain->DIM_X = x;
    domain->DIM_Y = y;
    domain->DIM_Z = z;
    domain->numParticles = numParticles;

    // Allocate memory for the particles
    domain->particles = (Particle*)malloc(numParticles * sizeof(Particle));

    // Compute chunks
    domain->chunkSize = 1.0f;

    // How many chunks do we need in each dimension?
    int chunksX = x / domain->chunkSize;
    int chunksY = y / domain->chunkSize;
    int chunksZ = z / domain->chunkSize;

    domain->chunkCounts[0] = chunksX;
    domain->chunkCounts[1] = chunksY;
    domain->chunkCounts[2] = chunksZ;

    printf("Chunks: %d %d %d\n", chunksX, chunksY, chunksZ);

    // Allocate memory for the chunks
    domain->chunks = (Chunk***)malloc(chunksX * sizeof(Chunk**));
    if (domain->chunks == NULL) {
        fprintf(stderr, "Memory allocation failed for chunks\n");
        exit(1);
    }
    for (int i = 0; i < chunksX; ++i) {
        domain->chunks[i] = (Chunk**)malloc(chunksY * sizeof(Chunk*));
        if (domain->chunks[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for chunks[%d]\n", i);
            exit(1);
        }
        for (int j = 0; j < chunksY; ++j) {
            domain->chunks[i][j] = (Chunk*)malloc(chunksZ * sizeof(Chunk));
            if (domain->chunks[i][j] == NULL) {
                fprintf(stderr, "Memory allocation failed for chunks[%d][%d]\n", i, j);
                exit(1);
            }
            for (int k = 0; k < chunksZ; ++k) {
                domain->chunks[i][j][k].numParticles = 0;
                domain->chunks[i][j][k].particles = (Particle**)malloc(numParticles * sizeof(Particle*));
                if (domain->chunks[i][j][k].particles == NULL) {
                    fprintf(stderr, "Memory allocation failed for chunks[%d][%d][%d].particles\n", i, j, k);
                    exit(1);
                }
                for (int l = 0; l < 6; ++l) {
                    domain->chunks[i][j][k].adj[l] = NULL;
                }
            }
        }
    }

    // Configure adjacency
    for (int i = 0; i < chunksX; ++i) {
        for (int j = 0; j < chunksY; ++j) {
            for (int k = 0; k < chunksZ; ++k) {
                Chunk *chunk = &domain->chunks[i][j][k];
                if (i > 0) {
                    chunk->adj[0] = &domain->chunks[i - 1][j][k];
                }
                if (i < chunksX - 1) {
                    chunk->adj[1] = &domain->chunks[i + 1][j][k];
                }
                if (j > 0) {
                    chunk->adj[2] = &domain->chunks[i][j - 1][k];
                }
                if (j < chunksY - 1) {
                    chunk->adj[3] = &domain->chunks[i][j + 1][k];
                }
                if (k > 0) {
                    chunk->adj[4] = &domain->chunks[i][j][k - 1];
                }
                if (k < chunksZ - 1) {
                    chunk->adj[5] = &domain->chunks[i][j][k + 1];
                }
            }
        }
    }
}

void updateChunks(Domain* domain) {
    const int DIM_X = domain->DIM_X;
    const int DIM_Y = domain->DIM_Y;
    const int DIM_Z = domain->DIM_Z;

    const float chunkSize = domain->chunkSize;

    // TODO: Optimize this

    // Clear all chunks
    for (int i = 0; i < domain->chunkCounts[0]; ++i) {
        for (int j = 0; j < domain->chunkCounts[1]; ++j) {
            for (int k = 0; k < domain->chunkCounts[2]; ++k) {
                domain->chunks[i][j][k].numParticles = 0;
            }
        }
    }

    // Update chunks
    for (int i = 0; i < domain->numParticles; ++i) {
        Particle *particle = &domain->particles[i];

        // Check if particle positions are within domain boundaries
        if (particle->pos[0] < 0 || particle->pos[0] >= DIM_X ||
            particle->pos[1] < 0 || particle->pos[1] >= DIM_Y ||
            particle->pos[2] < 0 || particle->pos[2] >= DIM_Z) {
            fprintf(stderr, "Particle %d position out of bounds: (%f, %f, %f)\n", i, particle->pos[0], particle->pos[1], particle->pos[2]);
            continue;
        }

        int chunkX = particle->pos[0] / chunkSize;
        int chunkY = particle->pos[1] / chunkSize;
        int chunkZ = particle->pos[2] / chunkSize;

        // Double-check that chunk indices are within bounds
        if (chunkX < 0 || chunkX >= domain->chunkCounts[0] ||
            chunkY < 0 || chunkY >= domain->chunkCounts[1] ||
            chunkZ < 0 || chunkZ >= domain->chunkCounts[2]) {
            fprintf(stderr, "Calculated chunk index out of bounds for particle %d: (%d, %d, %d)\n", i, chunkX, chunkY, chunkZ);
            continue;
        }

        Chunk *chunk = &domain->chunks[chunkX][chunkY][chunkZ];
        chunk->particles[chunk->numParticles] = particle;
        chunk->numParticles++;
    }
}
