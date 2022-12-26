#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_nor;
layout (location = 2) out vec2 out_uv;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1);
    out_pos = vec3(model * vec4(pos, 1));
    out_uv = uv;
    out_nor = mat3(transpose(inverse(model))) * nor;
}
