#version 460 core

in fData {
    vec4 color;
    vec3 normal;
} frag;

out vec4 color;

void main() {
	if(frag.color.a < 0.5f)
		discard;

	color = vec4(frag.color);
}