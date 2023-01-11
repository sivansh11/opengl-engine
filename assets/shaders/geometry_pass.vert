#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_nor;
layout (location = 2) out vec2 out_uv;
layout (location = 3) out vec4 out_fragPosLightSpace;
layout (location = 4) out mat3 out_TBN;
 
layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;
// layout (location = 3) uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1);
    out_pos = vec3(model * vec4(pos, 1));
    out_uv = uv;

    vec3 T = normalize(vec3(model * vec4(tangent, 0)));
    vec3 B = normalize(vec3(model * vec4(biTangent, 0)));
    vec3 N = normalize(vec3(model * vec4(nor, 0)));
    mat3 TBN = mat3(T, B, N);
    out_nor = mat3(transpose(inverse(model))) * nor;
    out_TBN = TBN;
    // out_fragPosLightSpace = lightSpaceMatrix * model * vec4(pos, 1.0);
}
