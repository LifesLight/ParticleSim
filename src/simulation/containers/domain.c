#include "simulation/containers/domain.h"

/**
 * Copyright (c) Alexander Kurtz 2024
 */


void initDomain(Domain* domain, int x, int y, int z) {
    domain->DIM_X = x;
    domain->DIM_Y = y;
    domain->DIM_Z = z;

    Cell defaultCell;
    defaultCell.isSet = false;

    // Allocate memory for 3D array
    domain->cells = (Cell***)malloc(domain->DIM_X * sizeof(Cell**));
    for(int i = 0; i < domain->DIM_X; i++) {
        domain->cells[i] = (Cell**)malloc(domain->DIM_Y * sizeof(Cell*));
        for(int j = 0; j < domain->DIM_Y; j++) {
            domain->cells[i][j] = (Cell*)malloc(domain->DIM_Z * sizeof(Cell));
            for(int k = 0; k < domain->DIM_Z; k++) {
                domain->cells[i][j][k] = defaultCell;
            }
        }
    }
}
