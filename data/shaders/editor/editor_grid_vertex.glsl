#version 150

uniform mat4 MVP;
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform vec3 CAM_POSITION;

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 pos;

void main(void) {
    gl_Position = (PROJECTION * MODEL * vec4(vertex_position, 1.0));
    pos = (PROJECTION * MODEL * vec4(vertex_position, 1.0)).xyz;
}