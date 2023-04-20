#version 460 core

layout (location = 0) in vec2 inPOs;
layout (location = 1) in vec2 inUv;

out fData {
	vec2 uv;
} frag;

void main() {
	frag.uv = inUv;
	gl_Position = vec4(inPOs, 0, 1);
}