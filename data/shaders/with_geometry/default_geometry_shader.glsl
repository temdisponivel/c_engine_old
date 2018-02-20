#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

uniform mat4 MVP;
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;

in vec3 v_pos[3];
in vec3 v_color[3];
in vec2 v_uv[3];

out vec3 pos;
out vec3 color;
out vec2 uv;

void main(void) {
    vec4 offset = vec4(1, 1, 1, 1);
    for (int i = 0; i < 1; i++) {
        gl_Position = gl_in[0].gl_Position + offset;
        color = v_color[0];
        uv = v_uv[0];
        pos = v_pos[0];
        EmitVertex();

        gl_Position = gl_in[1].gl_Position + offset;
        color = v_color[1];
        uv = v_uv[1];
        pos = v_pos[1];
        EmitVertex();

        gl_Position = gl_in[2].gl_Position + offset;
        color = v_color[2];
        uv = v_uv[2];
        pos = v_pos[2];
        EmitVertex();

        EndPrimitive();

        offset = offset * 1.2;
    }

/*
    for (int j = 0; j < 10; j++) {
        for(int i = 0; i < 3; i++) {
            gl_Position = PROJECTION * VIEW * MODEL * gl_in[i].gl_Position;
            gl_Position += vec4(offset * j, 0);

            out_color = color[i];// vec3(1, 1, 1);
            out_uv = uv[i];

            EmitVertex();
        }
        EndPrimitive();
    }
    */
}