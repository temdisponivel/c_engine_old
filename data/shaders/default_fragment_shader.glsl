#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in vec3 color;
in vec2 uv;
in vec3 pos;

out vec4 final_color;
uniform sampler2D my_texture;
uniform vec4 tint;
uniform vec2 offset;
uniform vec2 wrap;

void main(void) {
    final_color = texture(my_texture, (uv * wrap) + offset);
    //final_color = vec4(1, 1, 1,1 );
}