#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out fData {
    vec2 uv;
    vec3 T;
    vec3 B;
    vec3 N;
    vec3 worldPosition;
    vec4 lightSpacePosition;
} frag;

uniform mat4 model;
uniform mat4 invModel;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main() {
    mat3 invModelT = transpose(inverse(mat3(model)));
    frag.T = normalize(vec3(invModelT * tangent));
    frag.B = normalize(vec3(invModelT * biTangent));
    frag.N = normalize(vec3(invModelT * inNormal));

    frag.worldPosition = vec3(model * vec4(inPosition, 1));
    frag.lightSpacePosition = lightSpace * model * vec4(inPosition, 1);

    frag.uv = inUv;

    gl_Position = projection * view * vec4(frag.worldPosition, 1);
}
