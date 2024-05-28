#pragma once

/**
 * Copyright (c) Alexander Kurtz 2024
 */

#ifndef TERMINAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif
#include <iostream>
#include <cmath>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <vector>
#include <fstream>
#include <string>

#include "visualiser/common.hpp"
#include "simulation/containers/domain.h"

void startVisualiser();
