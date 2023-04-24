#version 460 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUv;

out fData {
    vec2 uv;
} frag;

void main() {
    gl_Position = vec4(inPosition, 0, 1);
    frag.uv = inUv;
}
