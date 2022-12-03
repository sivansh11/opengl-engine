#version 450 core

layout (location = 0) in vec2 uv;

out vec4 frag;

uniform sampler2D fb;

void main() {
    frag = texture(fb, uv);
}