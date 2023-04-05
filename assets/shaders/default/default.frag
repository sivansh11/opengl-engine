#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;

out vec4 fragment;

uniform struct Material {
    vec3 diffuseColor;
    vec3 
};