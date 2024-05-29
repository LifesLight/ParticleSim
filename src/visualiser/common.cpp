/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "visualiser/common.hpp"

Domain* getSimulationHandle(Config config) {
    Domain* renderDomain = new Domain();
    renderDomain->drawable = false;
    std::thread simulationThread(startSimulation, renderDomain, config);
    simulationThread.detach();
    return renderDomain;
}
