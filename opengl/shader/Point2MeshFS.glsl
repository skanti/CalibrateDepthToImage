#version 410

in vec3 position_vs;
in vec3 frag_color_vs;
out vec4 frag_color;

const vec4 pos_light0 = vec4(0, 0, 1, 1);
const vec3 la = vec3(0.1);
const vec3 ld = vec3(0.5);
const vec3 ls = vec3(0.6);


const vec3 ka = vec3(1.0, 1.0, 1.0);
const vec3 ks = vec3(1.0, 1.0, 1.0);
const float shininess = 1.0;

uniform mat4 model_matrix;
uniform mat4 view_matrix;

void main() {
    vec3 X = dFdx(position_vs);
    vec3 Y = dFdy(position_vs);
    vec3 normal = normalize(cross(X,Y));
    //vec3 normal1 = normalize(vec3(transpose(inverse(view_matrix*model_matrix))*vec4(normalize(normal), 1.0)));
    vec3 normal1 = normalize(vec3(view_matrix*vec4(normalize(normal), 0.0)));

    vec3 pos_light = vec3(view_matrix*pos_light0);
    vec3 s = normalize(pos_light - position_vs);
    vec3 v = normalize(-position_vs);
    vec3 r = reflect(-s, normal1);

    float sDotN = max(dot(s, normal1), 0.0);
    vec3 ambient = la*ka;
    vec3 diffuse = ld*frag_color_vs*sDotN;
    vec3 specular = vec3(0.0);
    if( sDotN > 0.0 )
        specular = ls*ks*pow(max(dot(r,v), 0.0), shininess);

    frag_color = vec4( diffuse + ambient + specular, 1 );
}

