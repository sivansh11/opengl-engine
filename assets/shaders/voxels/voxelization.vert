#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out vData {
    vec2 uv;
    vec4 position;
} geom;

uniform mat4 model;
uniform mat4 invModel;
// uniform mat4 view;
// uniform mat4 projection;
uniform mat4 lightSpace;

void main() {
    geom.uv = inUv;
    geom.position = lightSpace * model * vec4(inPosition, 1);
    geom.position.xyz = geom.position.xyz * 0.5f + 0.5f;
    gl_Position = model * vec4(inPosition, 1);
}
