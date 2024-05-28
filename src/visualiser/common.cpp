/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "visualiser/common.hpp"

Domain* getSimulationHandle(int size_x, int size_y, int size_z, int numParticles) {
    Domain* renderDomain = new Domain();
    renderDomain->drawable = false;
    std::thread simulationThread(startSimulation, renderDomain, numParticles, size_x, size_y, size_z);
    simulationThread.detach();
    return renderDomain;
}
