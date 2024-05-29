#version 120

attribute vec3 aPos;
attribute vec3 aColor;

uniform mat4 projection;
uniform mat4 view;

varying vec3 vColor;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    vColor = aColor;
}