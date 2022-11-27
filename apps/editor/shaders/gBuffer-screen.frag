#version 450 core

layout (location = 0) in vec2 uv;

out vec4 frag;

uniform sampler2D pos;
uniform sampler2D nor;
uniform sampler2D albedoSpec;


void main() {
    frag = texture(albedoSpec, uv);
}