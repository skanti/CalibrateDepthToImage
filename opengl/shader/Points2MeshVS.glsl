#version 450

layout (location = 0) in vec3 position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 position_vs;

void main() {
	mat4 mvp_matrix = projection_matrix*view_matrix*model_matrix;
    gl_Position = mvp_matrix*vec4(position, 1.0);
}

