#version 460 core

in fData {
	vec2 uv;
} frag;

out vec4 outColor;

uniform int voxelDimensions;
uniform float voxelGridSize;
uniform sampler3D voxels;
uniform vec3 cameraPosition; 
uniform mat4 invView;
uniform mat4 invProjection;

vec4 sampleVoxel(vec3 worldPosition, float lod);
vec4 coneTrace(vec3 startPos, vec3 direction, float tanHalfAngle, out float occlusion);
vec3 getWorldSpaceDirection(mat4 inverseProj, mat4 inverseView, vec2 normalizedDeviceCoords);

uniform sampler2D texDepth;

float perVoxelSize;

vec4 getViewPositionFromDepth(vec2 uv, float depth);

void main() {
    perVoxelSize = voxelGridSize / voxelDimensions;
	vec3 worldPosition = vec3(invView * getViewPositionFromDepth(frag.uv, texture(texDepth, frag.uv).r));
	vec3 dir = normalize(worldPosition - cameraPosition);
	vec3 startPos = cameraPosition + dir * perVoxelSize;
	float oc;
	vec3 color = coneTrace(startPos, dir, 0, oc).rgb;
	outColor = vec4(color, 1);
}

vec4 getViewPositionFromDepth(vec2 uv, float depth) {
    float z = depth * 2 - 1;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyzw;
}

vec3 getWorldSpaceDirection(mat4 inverseProj, mat4 inverseView, vec2 normalizedDeviceCoords) {
    vec4 rayEye = inverseProj * vec4(normalizedDeviceCoords, -1.0, 0.0);
    rayEye.zw = vec2(-1.0, 0.0);
    return normalize((inverseView * rayEye).xyz);
}

vec4 coneTrace(vec3 startPos, vec3 direction, float tanHalfAngle, out float occlusion) {
    float lod = 0;
    vec3 color = vec3(0);
    float alpha = 0;
    occlusion = 0;

    float dist = perVoxelSize;

    int count = 0;

    while (dist < 1000 && alpha < .99 && count < 1000) {
        float diameter = max(perVoxelSize, 2 * tanHalfAngle * dist);
        float lodLevel = log2(diameter / perVoxelSize);
        vec4 voxelColor = sampleVoxel(startPos + dist * direction, lodLevel);

        float a = (1 - alpha);
        color += a * voxelColor.rgb;
        alpha += a * voxelColor.a;

        occlusion += (a * voxelColor.a) / (1 + .03 * diameter);
        dist += diameter * .5;
        count += 1;
    }

    return vec4(color, alpha);
}

vec4 sampleVoxel(vec3 worldPosition, float lod) {
    const vec3 offset = vec3(1.0 / voxelDimensions, 1.0 / voxelDimensions, 0);
    vec3 voxelTextureUV = worldPosition / (voxelGridSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5 + offset;
    return textureLod(voxels, voxelTextureUV, lod);
}