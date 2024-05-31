#include "simulation/containers/chunk.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void initChunks(Domain *domain) {
    Config config = domain->config;

    // Compute chunks
    const int targetChunkCount = config.targetChunkCount;

    float totalVolume = config.dim[0] * config.dim[1] * config.dim[2];

    // Calculate the ideal volume of each chunk
    float idealChunkVolume = totalVolume / targetChunkCount;

    // Calculate the chunk size based on the ideal volume
    domain->chunkSize = cbrt(idealChunkVolume);

    // How many chunks do we need in each dimension?
    int chunksX = ceil(config.dim[0] / domain->chunkSize);
    int chunksY = ceil(config.dim[1] / domain->chunkSize);
    int chunksZ = ceil(config.dim[2] / domain->chunkSize);

    domain->chunkCounts[0] = chunksX;
    domain->chunkCounts[1] = chunksY;
    domain->chunkCounts[2] = chunksZ;

    const int defaultChunkStorage = config.numParticles / (chunksX * chunksY * chunksZ) + 1;

    printf("Chunk size: %f\n", domain->chunkSize);

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
                domain->chunks[i][j][k].size = defaultChunkStorage;
                domain->chunks[i][j][k].particles = (Particle**)malloc(defaultChunkStorage * sizeof(Particle*));
                if (domain->chunks[i][j][k].particles == NULL) {
                    fprintf(stderr, "Memory allocation failed for chunks[%d][%d][%d].particles\n", i, j, k);
                    exit(1);
                }
                for (int l = 0; l < 26; ++l) {
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

                // Iterate over neighboring indices
                for (int ni = -1; ni <= 1; ++ni) {
                    for (int nj = -1; nj <= 1; ++nj) {
                        for (int nk = -1; nk <= 1; ++nk) {
                            // Skip the current chunk itself
                            if (ni == 0 && nj == 0 && nk == 0)
                                continue;

                            // Calculate neighbor indices
                            int ni_index = i + ni;
                            int nj_index = j + nj;
                            int nk_index = k + nk;

                            // Check if neighbor indices are within bounds
                            if (ni_index >= 0 && ni_index < chunksX &&
                                nj_index >= 0 && nj_index < chunksY &&
                                nk_index >= 0 && nk_index < chunksZ) {
                                // Access neighbor chunk
                                Chunk *neighbor = &domain->chunks[ni_index][nj_index][nk_index];

                                // Add neighbor to adjacency list
                                chunk->adj[3 * (ni + 1) + (nj + 1) * 3 + (nk + 1)] = neighbor;
                            }
                        }
                    }
                }
            }
        }
    }
}

void resizeParticleChunk(Chunk *chunk) {
    if (chunk->numParticles >= chunk->size) {
        Particle **newParticles = NULL;
        int newSize = chunk->size * 2;

        newParticles = (Particle**)realloc(chunk->particles, newSize * sizeof(Particle*));

        if (newParticles == NULL) {
            fprintf(stderr, "Memory allocation failed for chunk resize %d\n", newSize);
            exit(1);
        }

        chunk->particles = newParticles;
        chunk->size = newSize;
    }
}


void updateChunks(Domain* domain) {
    const int DIM_X = domain->config.dim[0];
    const int DIM_Y = domain->config.dim[1];
    const int DIM_Z = domain->config.dim[2];

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
    for (int i = 0; i < domain->config.numParticles; ++i) {
        Particle *particle = &domain->particles[i];

        // Check if particle positions are within domain boundaries
        if (particle->pos.x < 0 || particle->pos.x >= DIM_X ||
            particle->pos.y < 0 || particle->pos.y >= DIM_Y ||
            particle->pos.z < 0 || particle->pos.z >= DIM_Z) {
            fprintf(stderr, "Particle %d position out of bounds: (%f, %f, %f)\n", i, particle->pos.x, particle->pos.y, particle->pos.z);
            exit(1);
        }

        const V3 temp = div3(&particle->pos, chunkSize);

        const int chunkX = temp.x;
        const int chunkY = temp.y;
        const int chunkZ = temp.z;

        // Double-check that chunk indices are within bounds
        if (chunkX < 0 || chunkX >= domain->chunkCounts[0] ||
            chunkY < 0 || chunkY >= domain->chunkCounts[1] ||
            chunkZ < 0 || chunkZ >= domain->chunkCounts[2]) {
            fprintf(stderr, "Calculated chunk index out of bounds for particle %d: (%d, %d, %d)\n", i, chunkX, chunkY, chunkZ);
            exit(1);
        }

        Chunk *chunk = &domain->chunks[chunkX][chunkY][chunkZ];

        // Resize the chunk if necessary
        resizeParticleChunk(chunk);

        chunk->particles[chunk->numParticles] = particle;
        chunk->numParticles++;
    }
}
