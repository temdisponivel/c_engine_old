#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in vec3 color;
in vec2 uv;

out vec4 final_color;
uniform sampler2D my_texture;
uniform vec4 tint;
uniform vec2 offset;
uniform vec2 wrap;

void main(void) {
    vec4 tex_color = texture(my_texture, (uv * wrap) + offset);
    final_color = tex_color * tint * vec4(color, 1);
    final_color = final_color + vec4(1, 1, 1, 1);
}