/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "simulation/start.h"

void startSimulation(Domain* visualizerDomain) {
    Domain domain;

    initDomain(&domain, 10, 10, 10);

    // Set some cells randomly to alive
    srand(time(NULL));
    for(int i = 0; i < domain.DIM_X; i++) {
        for(int j = 0; j < domain.DIM_Y; j++) {
            for(int k = 0; k < domain.DIM_Z; k++) {
                domain.cells[i][j][k].isSet = rand() % 2;
            }
        }
    }

    domain.drawable = true;

    // Copy domain to visualizer domain
    memccpy(visualizerDomain, &domain, sizeof(Domain), sizeof(Domain));
}
