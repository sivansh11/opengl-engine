#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_nor;
layout (location = 2) out vec2 out_uv;


void main() {
    out_pos = pos;
    out_nor = nor;
    out_uv = uv;

    gl_Position = vec4(pos, 1);
}