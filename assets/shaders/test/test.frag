#version 460

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout (r32ui) uniform uimage2D something;

void main() {
    imageAtomicCompSwap(something, ivec2(1, 2), uint(1), uint(1));
    outColor = vec4(fragColor, 1.0);
}