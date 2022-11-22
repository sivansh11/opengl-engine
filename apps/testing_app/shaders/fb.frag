#version 450 core

layout (location = 0) in vec2 uv;

out vec4 frag;

uniform sampler2D tex;

void main() {
    vec4 f = texture(tex, uv);
    // frag = vec4(1 - f.x, 1 - f.y, 1 - f.z, f.a);
    frag = f;
}