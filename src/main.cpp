#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;
float camera_yaw = 0.0f;
float camera_pitch = 0.0f;
float camera_radius = 3.0f;
float camera_speed = 0.1f;
bool mouse_pressed = false;
double last_mouse_x = 0.0;
double last_mouse_y = 0.0;

#define DIM_X 15
#define DIM_Y 15
#define DIM_Z 15

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

        // Clamp pitch to avoid flipping
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
    // Draw a simple colored cube with normals for lighting
    float halfSize = size / 2.0f;
    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);
    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);
    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glEnd();
}


void drawCubes(bool cubes[DIM_X][DIM_Y][DIM_Z], float bufferFraction) {
    // Calculate the size of each cube and the gap between them
    float cubeSize = 1.0f / (DIM_X * (1.0f + bufferFraction) - bufferFraction);
    float gap = cubeSize * (1 - bufferFraction);

    // Calculate the total size of the cube tensor
    float tensorSize = (DIM_X * cubeSize) + ((DIM_X - 1) * gap);

    // Calculate the starting position to center the cube tensor
    float startX = -tensorSize / 2.0f;
    float startY = -tensorSize / 2.0f;
    float startZ = -tensorSize / 2.0f;

    for (int x = 0; x < DIM_X; ++x) {
        for (int y = 0; y < DIM_Y; ++y) {
            for (int z = 0; z < DIM_Z; ++z) {
                if (cubes[x][y][z]) {
                    // Calculate the position of the cube
                    float cubeX = startX + (x * (cubeSize + gap));
                    float cubeY = startY + (y * (cubeSize + gap));
                    float cubeZ = startZ + (z * (cubeSize + gap));

                    glPushMatrix();
                    glTranslatef(cubeX, cubeY, cubeZ);

                    glBegin(GL_QUADS);

                    drawCube(cubeSize); 

                    glPopMatrix();
                }
            }
        }
    }
}

int main() {
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WIDTH, HEIGHT, "ParticleSim", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable lighting and set light properties
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

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    bool cubes[DIM_X][DIM_Y][DIM_Z] = {0};

    double lastTime = glfwGetTime();
    double interval = 1.0;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Update camera position
        float camX = std::cos(glm::radians(camera_yaw)) * std::cos(glm::radians(camera_pitch)) * camera_radius;
        float camY = std::sin(glm::radians(camera_pitch)) * camera_radius;
        float camZ = std::sin(glm::radians(camera_yaw)) * std::cos(glm::radians(camera_pitch)) * camera_radius;

        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // Set background color
        glClearColor(0.1f, 0.1f, 0.4f, 1.0f);

        // Draw the cubes
        if (glfwGetTime() - lastTime > interval) {
            memset(cubes, 0, sizeof(cubes));
            lastTime = glfwGetTime();
            for (int i = 0; i < 40; ++i) {
                int x = rand() % DIM_X;
                int y = rand() % DIM_Y;
                int z = rand() % DIM_Z;
                cubes[x][y][z] = !cubes[x][y][z];
            }
        }

        drawCubes(cubes, 1.0f);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
