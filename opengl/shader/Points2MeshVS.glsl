#version 450

layout (location = 0) in vec3 position;

out vec3 position_vs;

void main() {
    gl_Position = vec4(position, 1.0);
}

