#version 450 core

#define POINT_LIGHT_INTENSITY 1
#define MAX_LIGHTS 1

#define DIST_FACTOR 1.1f
#define CONSTANT 1
#define LINEAR 0
#define QUADRATIC 1

float attenuate(float dist){ dist *= DIST_FACTOR; return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); }

struct PointLight {
	vec3 position;
	vec3 color;
};

struct Material {
	vec3 diffuseColor;
	vec3 specularColor;
	float reflectivity;
	float emissivity;
	float transparency; 
};

uniform Material material;
uniform PointLight pointLights;
uniform int numberOfLights;
layout (binding = 5, rgba8) uniform image3D voxels;

layout (location = 0) in vec3 in_worldPos;
layout (location = 1) in vec3 in_nor;

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

vec3 calculatePointLight(PointLight pointLight);

void main() {
	vec3 color = vec3(0);

	if (!isInsideCube(in_worldPos, 0)) return;

	int maxLights = min(numberOfLights, MAX_LIGHTS);

	for (int i = 0; i < maxLights; i++) {
		color += calculatePointLight(pointLights);
	}

	vec3 spec = material.reflectivity * material.specularColor;
	vec3 diff = material.reflectivity * material.diffuseColor;

	color = (diff + spec) * color + clamp(material.emissivity, 0, 1) * material.diffuseColor;

	vec3 voxel = scaleAndBias(in_worldPos);
	ivec3 dim = imageSize(voxels);
	float alpha = pow(1 - material.transparency, 4);

	vec4 res = alpha * vec4(color, 1);
	imageStore(voxels, ivec3(dim * voxel), res);
}

vec3 calculatePointLight(PointLight pointLight) {
	vec3 direction = normalize(pointLight.position - in_worldPos);
	float distToLight = distance(pointLight.position, in_worldPos);
	float attenuation = attenuate(distToLight);
	float d = max(dot(normalize(in_nor), direction), 0);
	return d * POINT_LIGHT_INTENSITY * attenuation * pointLight.color;
}