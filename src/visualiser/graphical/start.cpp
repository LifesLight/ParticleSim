#include "visualiser/start.hpp"

/**
 * Copyright (c) Alexander Kurtz 2024
 */

const int WIDTH = 1000;
const int HEIGHT = 1000;
const double FPS = 120.0;
const double frameDuration = 1.0 / FPS;

float camera_yaw = 0.0f;
float camera_pitch = 0.0f;
float camera_radius = 10.0f;
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


std::string readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
    }

    return shader;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Bind attribute location
    glBindAttribLocation(shaderProgram, 0, "aPos");

    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void updatePoints(const Particle* particles, size_t numParticles, std::vector<glm::vec3>& cords, std::vector<glm::vec3>& cols, int DIM_X, int DIM_Y, int DIM_Z, float speed) {
    cords.clear();
    cols.clear();
    for (size_t i = 0; i < numParticles; i++) {
        cords.push_back(glm::vec3(particles[i].pos.z - DIM_Z / 2, particles[i].pos.y - DIM_Y / 2, particles[i].pos.x - DIM_X / 2));
//        cols.push_back(glm::vec3(particles[i].col[0], particles[i].col[1], particles[i].col[2]) / 255.0f);

        float totalVel = sqrt(particles[i].vel.x * particles[i].vel.x + particles[i].vel.y * particles[i].vel.y + particles[i].vel.z * particles[i].vel.z);
        totalVel /= (speed * 500);

        if (totalVel < 0.0f) {
            totalVel *= -1;
        }

        float blue, red;

        totalVel *= 8;

        // Go smmoothly from blue to red
        if (totalVel < 0.5) {
            blue = 1.0f;
            red = 2.0f * totalVel;
        } else {
            blue = 2.0f - 2.0f * totalVel;
            red = 1.0f;
        }

        cols.push_back(glm::vec3(red, particles[i].col[1] / 255.0f, blue));

    }
}

void drawChunkBorder(int DIM_X, int DIM_Y, int DIM_Z) {
    // Calculate the half dimensions with rounding
    float halfDimX = DIM_X / 2.0f;
    float halfDimY = DIM_Y / 2.0f;
    float halfDimZ = DIM_Z / 2.0f;

    // Apply rounding
    float xNeg = ceil(-halfDimX);
    float xPos = ceil(halfDimX);
    float yNeg = ceil(-halfDimY);
    float yPos = ceil(halfDimY);
    float zNeg = ceil(-halfDimZ);
    float zPos = ceil(halfDimZ);

    // Draw the border of the chunk
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Front face
    glVertex3f(zNeg, yNeg, xNeg);
    glVertex3f(zPos, yNeg, xNeg);

    glVertex3f(zPos, yNeg, xNeg);
    glVertex3f(zPos, yPos, xNeg);

    glVertex3f(zPos, yPos, xNeg);
    glVertex3f(zNeg, yPos, xNeg);

    glVertex3f(zNeg, yPos, xNeg);
    glVertex3f(zNeg, yNeg, xNeg);

    // Back face
    glVertex3f(zNeg, yNeg, xPos);
    glVertex3f(zPos, yNeg, xPos);

    glVertex3f(zPos, yNeg, xPos);
    glVertex3f(zPos, yPos, xPos);

    glVertex3f(zPos, yPos, xPos);
    glVertex3f(zNeg, yPos, xPos);

    glVertex3f(zNeg, yPos, xPos);
    glVertex3f(zNeg, yNeg, xPos);

    // Connect front and back faces
    glVertex3f(zNeg, yNeg, xNeg);
    glVertex3f(zNeg, yNeg, xPos);

    glVertex3f(zPos, yNeg, xNeg);
    glVertex3f(zPos, yNeg, xPos);

    glVertex3f(zPos, yPos, xNeg);
    glVertex3f(zPos, yPos, xPos);

    glVertex3f(zNeg, yPos, xNeg);
    glVertex3f(zNeg, yPos, xPos);

    glEnd();
}



void startVisualiser() {
    GLFWwindow* window;

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
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
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    GLuint shaderProgram = createShaderProgram("../shaders/vertex_shader.glsl", "../shaders/fragment_shader.glsl");

    std::cout << "Compiled shader program." << std::endl;

    Config config;
    config.dim[0] = 75;
    config.dim[1] = 25;
    config.dim[2] = 15;

    config.friction = 0.5f;
    config.repulsion = 0.025f;

    config.gravity = {0.0f, -0.01f, 0.0f}; 
    config.speed = 0.01f;
    config.supsampling = 1;
    config.fps = 60;

    config.numParticles = 20000;
    config.radius = 0.5f;
    config.targetChunkCount = pow(4, 9);

    Domain* renderDomain = getSimulationHandle(config);

    while (!renderDomain->drawable) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    const Particle *particles = renderDomain->particles;
    std::vector<glm::vec3> cords;
    std::vector<glm::vec3> colors;

    updatePoints(particles, config.numParticles, cords, colors, config.dim[0], config.dim[1], config.dim[2], config.speed);

    GLuint VBO, CBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cords.size() * sizeof(glm::vec3), cords.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    double lastTime = glfwGetTime();

    std::cout << "Starting Render Loop..." << std::endl;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime >= frameDuration) {
            lastTime = currentTime;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Set background color
            glClearColor(0.831372549, 0.7960784314f, 0.8980392157f, 1.0f);

            if (renderDomain->drawable) {
                updatePoints(particles, config.numParticles, cords, colors, config.dim[0], config.dim[1], config.dim[2], config.speed);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, cords.size() * sizeof(glm::vec3), cords.data());

                glBindBuffer(GL_ARRAY_BUFFER, CBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(glm::vec3), colors.data());
            }

            float camX = std::cos(glm::radians(camera_yaw)) * std::cos(glm::radians(camera_pitch)) * camera_radius;
            float camY = std::sin(glm::radians(camera_pitch)) * camera_radius;
            float camZ = std::sin(glm::radians(camera_yaw)) * std::cos(glm::radians(camera_pitch)) * camera_radius;

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            drawChunkBorder(config.dim[0], config.dim[1], config.dim[2]);

            glUseProgram(shaderProgram);

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, CBO);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glPointSize((config.radius * 1000) / sqrt(camera_radius));
            glDrawArrays(GL_POINTS, 0, cords.size());

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glfwSwapBuffers(window);
        }

        glfwWaitEventsTimeout(frameDuration - (glfwGetTime() - lastTime));
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}
