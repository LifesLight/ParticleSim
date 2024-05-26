cmake_minimum_required(VERSION 3.20)
project(ParticleSim)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GLEAM library using pkg-config
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLFW REQUIRED glfw3)

# Debug messages
message(STATUS "GLEW_FOUND: ${GLEW_FOUND}")
message(STATUS "GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
message(STATUS "GLEW_LIBRARIES: ${GLEW_LIBRARIES}")
message(STATUS "GLFW_FOUND: ${GLFW_FOUND}")
message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")

# Manually set GLEW_LIBRARIES and GLFW_LIBRARIES to the full path of the library files
set(GLEW_LIBRARIES "/opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.dylib")
set(GLFW_LIBRARIES "/opt/homebrew/Cellar/glfw/3.4/lib/libglfw.dylib")

# Add include directories
include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} "include")

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(ParticleSim ${SOURCES})

# Link GLEAM library
target_link_libraries(ParticleSim ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

# Extra flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
