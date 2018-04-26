#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 position_vs;
out vec3 frag_color_vs;

void main() {
	mat4 mvp_matrix = projection_matrix*view_matrix*model_matrix;
    gl_Position = mvp_matrix*vec4(position, 1.0);
	position_vs = vec3(view_matrix*vec4(position, 1.0));
    frag_color_vs = color;
}

