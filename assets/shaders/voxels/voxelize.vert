#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;

layout (location = 0) out vec3 out_worldPos;
layout (location = 1) out vec3 out_nor;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    out_worldPos = vec3(model * vec4(pos, 1));
    out_nor = normalize(mat3(transpose(inverse(model))) * nor);
    gl_Position = projection * view * vec4(out_worldPos, 1);
}
