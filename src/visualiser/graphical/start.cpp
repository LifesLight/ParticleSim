/**
 * Copyright (c) Alexander Kurtz 2024
 */

#include "visualiser/start.hpp"

const int WIDTH = 1000;
const int HEIGHT = 1000;
const double FPS = 30.0;
const double frameDuration = 1.0 / FPS;

float camera_yaw = 0.0f;
float camera_pitch = 0.0f;
float camera_radius = 3.0f;
float camera_speed = 0.1f;
bool mouse_pressed = false;
double last_mouse_x = 0.0;
double last_mouse_y = 0.0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_pressed = true;
            glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
        } else if (action == GLFW_RELEASE) {
            mouse_pressed = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse_pressed) {
        float sensitivity = 0.05f;
        float xoffset = (float)(xpos - last_mouse_x) * sensitivity;
        float yoffset = (float)(ypos - last_mouse_y) * sensitivity;

        camera_yaw += xoffset;
        camera_pitch += yoffset;

        if (camera_pitch > 89.0f)
            camera_pitch = 89.0f;
        if (camera_pitch < -89.0f)
            camera_pitch = -89.0f;

        last_mouse_x = xpos;
        last_mouse_y = ypos;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera_radius -= yoffset;
    if (camera_radius < 0.1f)
        camera_radius = 0.1f;
}

void drawCube(float size) {
    float halfSize = size / 2.0f;
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glEnd();
}

void drawCubes(Domain* domain, float bufferFraction) {
    float cubeSize = 1.0f / (domain->DIM_X * (1.0f + bufferFraction) - bufferFraction);
    float gap = cubeSize * (1 - bufferFraction);
    float tensorSize = (domain->DIM_X * cubeSize) + ((domain->DIM_X - 1) * gap);
    float startX = -tensorSize / 2.0f;
    float startY = -tensorSize / 2.0f;
    float startZ = -tensorSize / 2.0f;

    for (int x = 0; x < domain->DIM_X; ++x) {
        for (int y = 0; y < domain->DIM_Y; ++y) {
            for (int z = 0; z < domain->DIM_Z; ++z) {
                if (domain->cells[x][y][z].isSet == true) {
                    float cubeX = startX + (x * (cubeSize + gap));
                    float cubeY = startY + (y * (cubeSize + gap));
                    float cubeZ = startZ + (z * (cubeSize + gap));

                    glPushMatrix();
                    glTranslatef(cubeX, cubeY, cubeZ);
                    drawCube(cubeSize);
                    glPopMatrix();
                }
            }
        }
    }
}

void startVisualiser() {
    GLFWwindow* window;

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "ParticleSim", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    Domain* renderDomain = new Domain();
    renderDomain->drawable = false;
    std::thread simulationThread(startSimulation, renderDomain);
    simulationThread.detach();

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime >= frameDuration) {
            lastTime = currentTime;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float camX = std::cos(glm::radians(camera_yaw)) * std::cos(glm::radians(camera_pitch)) * camera_radius;
            float camY = std::sin(glm::radians(camera_pitch)) * camera_radius;
            float camZ = std::sin(glm::radians(camera_yaw)) * std::cos(glm::radians(camera_pitch)) * camera_radius;

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(glm::value_ptr(projection));
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(view));

            glClearColor(0.1f, 0.1f, 0.4f, 1.0f);

            if (renderDomain->drawable) {
                drawCubes(renderDomain, 1.0f);
            }

            glfwSwapBuffers(window);
        }

        glfwWaitEventsTimeout(frameDuration - (glfwGetTime() - lastTime));
        glfwPollEvents();
    }

    glfwTerminate();
}
