#version 450

out vec4 frag_color;

void main() {
	float z_linear = gl_FragCoord.z;
	//int z_linear_int = int(gl_FragCoord.z *float(1 << 24));
    frag_color = vec4( z_linear, z_linear, z_linear, 1 );
}

