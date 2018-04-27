#version 450

out vec4 frag_color;

void main() {
	float z_nonlinear = gl_FragCoord.z;
    frag_color = vec4( z_nonlinear, z_nonlinear, z_nonlinear, 1 );
}

