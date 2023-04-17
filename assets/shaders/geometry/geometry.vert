#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUv;
layout (location = 3) out mat3 outTBN;

uniform mat4 model;
uniform mat4 invModel;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 T = normalize(vec3(model * vec4(tangent, 0)));
    vec3 B = normalize(vec3(model * vec4(biTangent, 0)));
    vec3 N = normalize(vec3(model * vec4(inNormal, 0)));
    mat3 TBN = mat3(T, B, N);

    vec3 outPosition = vec3(model * vec4(inPosition, 1));
    outNormal = mat3(transpose(invModel)) * inNormal;
    outUv = inUv;
    outTBN = TBN;

    gl_Position = projection * view * vec4(outPosition, 1);
}
