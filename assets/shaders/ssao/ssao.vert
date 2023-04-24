#version 460 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUv;

layout (location = 0) out vec2 outUv;

out fData {
    vec2 uv;
} frag;

void main() {
    frag.uv = inUv;
    gl_Position = vec4(inPosition, 0, 1);
}
