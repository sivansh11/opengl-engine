#version 450 core

layout (location = 0) in vec3 pos;

layout (location = 0) out vec2 out_worldPos;

vec2 scaleAndBias(vec2 p) { return 0.5f * p + vec2(0.5f); }

void main() {
	out_worldPos = scaleAndBias(pos.xy);
	gl_Position = vec4(pos, 1);
}
