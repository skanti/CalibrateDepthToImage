#version 410 core

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

in vec3 frag_color_vs[];

out vec3 frag_color_gs;

void main() {
    frag_color_gs = frag_color_vs[0];

    float a = 0.003;
    mat4 MVPMatrix = projection_matrix*view_matrix*model_matrix;
	float x = gl_in[0].gl_Position.x;
    float y = gl_in[0].gl_Position.y;
	float z = gl_in[0].gl_Position.z;
    gl_Position = MVPMatrix*vec4(x - a, y - a, z, 1.0);    
	EmitVertex();
    gl_Position = MVPMatrix*vec4(x + a, y - a, z, 1.0);
	EmitVertex();
    gl_Position = MVPMatrix*vec4(x - a, y + a, z, 1.0);
	EmitVertex();
    gl_Position = MVPMatrix*vec4(x + a, y + a, z, 1.0);
	EmitVertex();
    EndPrimitive();

}
