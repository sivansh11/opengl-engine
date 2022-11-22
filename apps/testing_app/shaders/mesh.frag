#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;

out vec4 frag;

void main() {
    frag = vec4(1, 1, 1, 1);
}