#include "visualiser/start.hpp"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

void renderDomainSlice(Domain* domain, int slice) {

    for (int i = 0; i < domain->DIM_X; i++) {
        std::cout << "--";
    }

    std::cout << std::endl;

    for(int i = 0; i < domain->DIM_X; i++) {
        for(int j = 0; j < domain->DIM_Y; j++) {
            if(domain->cells[i][j][slice].isSet) {
                std::cout << "X ";
            } else {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < domain->DIM_X; i++) {
        std::cout << "--";
    }

    std::cout << std::endl;
}

void startVisualiser() {
    Domain* renderDomain = new Domain();
    renderDomain->drawable = false;
    std::thread simulationThread(startSimulation, renderDomain);
    simulationThread.detach();

    while(true) {
        if(renderDomain->drawable) {
            renderDomainSlice(renderDomain, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}