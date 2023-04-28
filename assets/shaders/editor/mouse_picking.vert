#version 460 core

layout (location = 0) in vec3 inPosition;

uniform mat4 model;
uniform mat4 invModel;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 position = vec3(model * vec4(inPosition, 1));
    gl_Position = projection * view * vec4(position, 1);
}
