#version 450 core

#define INV_STEP_LENGTH (1.0f/STEP_LENGTH)
#define STEP_LENGTH 0.005f

uniform sampler2D textureBack;
uniform sampler2D textureFront;
uniform sampler3D voxels;
uniform vec3 cameraPosition;
uniform int state;

layout (location = 0) in vec2 textureCoordinate;

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

// Returns true if p is inside the unity cube (+ e) centered on (0, 0, 0).
bool isInsideCube(vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main() {
	float mipmapLevel = state;

	vec3 origin = isInsideCube(cameraPosition, 0.2) ? cameraPosition : texture(textureFront, textureCoordinate).xyz;

	vec3 direction = texture(textureBack, textureCoordinate).xyz - origin;
}
