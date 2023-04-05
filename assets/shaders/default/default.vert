#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    outPosition = vec3(model * vec4(inPosition, 1));
    outNormal = inNormal;
    outUv = inUv;

    gl_Position = projection * view * vec4(outPosition, 1);
}


