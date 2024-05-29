#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include <thread>

#include "simulation/containers/domain.h"
#include "simulation/start.h"

using std::thread;

Domain* getSimulationHandle(Config config);
