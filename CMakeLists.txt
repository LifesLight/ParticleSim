cmake_minimum_required(VERSION 3.20)
project(ParticleSim)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_C_STANDARD_REQUIRED OFF)

# Define the compile variant option
option(TERMINAL "Compile terminal variant" OFF)

# Common source files
set(SOURCES_COMMON
    src/main.cpp
    src/simulation/containers/domain.c
    src/simulation/start.c
    src/visualiser/common.cpp
)

if (TERMINAL)
    message(STATUS "COMPILE TERMINAL VARIANT")
    file(GLOB SOURCES_VARIANT "src/visualiser/terminal/*.cpp")
    add_executable(ParticleSim ${SOURCES_COMMON} ${SOURCES_VARIANT})

    include_directories(include)

    target_compile_definitions(ParticleSim PRIVATE TERMINAL)
else()
    message(STATUS "COMPILE GRAPHICAL VARIANT")

    # Find GLEW, GLFW, GLM libraries using pkg-config
    find_package(PkgConfig REQUIRED)
    find_package(OpenGL REQUIRED)
    pkg_check_modules(GLEW REQUIRED glew)
    pkg_check_modules(GLFW REQUIRED glfw3)
    pkg_check_modules(GLM REQUIRED glm)

    # Debug messages
    message(STATUS "GLEW_FOUND: ${GLEW_FOUND}")
    message(STATUS "GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
    message(STATUS "GLEW_LIBRARIES: ${GLEW_LIBRARIES}")
    message(STATUS "GLFW_FOUND: ${GLFW_FOUND}")
    message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
    message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")
    message(STATUS "GLM_FOUND: ${GLM_FOUND}")
    message(STATUS "GLM_INCLUDE_DIRS: ${GLM_INCLUDE_DIRS}")
    message(STATUS "GLM_LIBRARIES: ${GLM_LIBRARIES}")

    file(GLOB SOURCES_VARIANT "src/visualiser/graphical/*.cpp")

    # Include directories
    include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} include)

    add_executable(ParticleSim ${SOURCES_COMMON} ${SOURCES_VARIANT})
    target_compile_definitions(ParticleSim PRIVATE GRAPHICAL)

    # Include directories
    target_include_directories(ParticleSim PRIVATE ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

    # Link directories
    target_link_directories(ParticleSim PRIVATE ${GLEW_LIBRARY_DIRS} ${GLFW_LIBRARY_DIRS})

    # Link libraries
    target_link_libraries(ParticleSim PRIVATE ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES})
endif()
# Extra flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fsanitize=address -fno-omit-frame-pointer -g")

# Debug messages
message(STATUS "Compile variant: ${VARIANT}")
