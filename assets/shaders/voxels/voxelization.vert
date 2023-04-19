#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out vData {
    vec2 uv;
    vec4 lightSpacePosition;
    vec3 worldPosition;
    vec3 T;
    vec3 B;
    vec3 N;
} geom;

uniform mat4 model;
uniform mat4 invModel;
uniform mat4 lightSpace;

void main() {
    mat3 invModelT = transpose(inverse(mat3(model)));
    geom.T = normalize(vec3(invModelT * tangent));
    geom.B = normalize(vec3(invModelT * biTangent));
    geom.N = normalize(vec3(invModelT * inNormal));

    geom.uv = inUv;
    geom.lightSpacePosition = lightSpace * model * vec4(inPosition, 1);
    geom.lightSpacePosition.xyz = geom.lightSpacePosition.xyz * 0.5f + 0.5f;
    geom.worldPosition = vec3(model * vec4(inPosition, 1));
    gl_Position = model * vec4(inPosition, 1);
}
