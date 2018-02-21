#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

#define PI 3.141592
#define N 30

out vec4 color;
in vec3 pos;

void main(void) {
    vec3 coord = cos(PI/N*pos);
    color = vec4(0.0, 0, 0, 1) + 0.5 * smoothstep(0.95,1.0,max(coord.x, coord.z));
    //color = vec4(1, 1, 1, 1);
}