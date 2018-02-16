#version 150

uniform mat4 MVP;
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;

in vec3 vertex_position;
in vec3 vertex_color;
in vec3 vertex_normal;
in vec2 vertex_texture_coord;

out vec3 v_pos;
out vec3 v_color;
out vec2 v_uv;

void main(void) {
    gl_Position = PROJECTION * VIEW * MODEL * vec4(vertex_position, 1.0);

    v_color = vertex_color;// vec3(1, 1, 1);
    v_uv = vertex_texture_coord;
    v_pos = vertex_position;
}