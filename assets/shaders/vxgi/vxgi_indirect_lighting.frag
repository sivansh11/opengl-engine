#version 460 core

#define PI 3.14159265

in fData {
    vec2 uv;
} frag;

out vec4 outColor;

uniform sampler2DShadow shadowMap;
uniform sampler2D texNoise;
uniform mat4 invProjection;
uniform mat4 invView;
uniform mat4 lightSpace;
uniform float voxelGridSize;
uniform float tanHalfAngle;
uniform float ALPHA_THRESH;
uniform float MAX_DIST;
uniform int voxelDimensions;
uniform int samples;
uniform int MAX_COUNT;
uniform sampler2D texDepth;
uniform sampler2D texNormal;
uniform sampler3D voxels;
uniform vec3 cameraPosition;

vec4 coneTrace(vec3 startPos, vec3 direction, float tanHalfAngle, out float occlusion);
vec4 sampleVoxel(vec3 worldPosition, float lod);

// random function generator
uint pcg_hash(uint seed);
uint rand_pcg(inout uint rng_state);
float rand(inout uint state);
vec3 rand_vec3(inout uint seed);
float rand(inout uint seed, float min, float max);
vec3 rand_vec3(inout uint seed, float min, float max);
vec3 random_in_unit_sphere(inout uint seed);
vec3 random_unit_vector(inout uint seed);
vec4 get_view_position_from_depth(vec2 uv, float depth);
vec3 calculateLight(int index);
vec3 uniformSampleSphere(inout uint seed);
vec3 cosineSampleHemiSphere(inout uint seed, vec3 normal);

vec4 viewPosition;
vec4 worldPosition;
float perVoxelSize;
vec3 normal;
vec3 tangent;
vec3 biTangent;
mat3 TBN;
vec3 up;
vec3 forward;
vec3 startPos;

void main() {
    up = vec3(0, 1, 0);
    viewPosition = get_view_position_from_depth(frag.uv, texture(texDepth, frag.uv).r);
    worldPosition = invView * viewPosition;
    perVoxelSize = voxelGridSize / voxelDimensions;
    normal = texture(texNormal, frag.uv).rgb;
    startPos = worldPosition.rgb + normal * perVoxelSize * 1;

    uint pixel_index = uint(gl_FragCoord.x + gl_FragCoord.y * 10000);
    uint seed = pcg_hash(pixel_index);
    // uint seed = uint(texture(texNoise, frag.uv).r);

    vec3 indirectLight = vec3(0);
    // indirect light 
    for (int i = 0; i < samples; i++) {
        float tempOcclusion = 0; 
        indirectLight += coneTrace(startPos, cosineSampleHemiSphere(seed, normal), tanHalfAngle, tempOcclusion).rgb / float(samples);
        // vec3 r = random_in_unit_sphere(seed);
        // if (dot(r, normal) < 0) r = -r;
        // r /= 10;
        // indirectLight += coneTrace(startPos, r, tanHalfAngle, tempOcclusion).rgb / float(samples);
        // indirectLight += coneTrace(startPos, normal + r, tanHalfAngle, tempOcclusion).rgb / float(samples);
        // indirectLight += coneTrace(startPos, normal, tanHalfAngle, tempOcclusion).rgb / float(samples);
    }
    
    outColor = vec4(indirectLight, 1);
    // float random = rand(seed);
    // outColor = vec4(random, random, random, 1);
}

vec3 uniformSampleSphere(inout uint seed) {
    float z = rand(seed) * 2 - 1;
    float a = rand(seed) * 2 * PI;
    float r = sqrt(1 - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

vec3 cosineSampleHemiSphere(inout uint seed, vec3 normal) {
    return normalize(normal + uniformSampleSphere(seed));
}

vec4 get_view_position_from_depth(vec2 uv, float depth) {
    float z = depth * 2 - 1;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyzw;
}

vec4 coneTrace(vec3 startPos, vec3 direction, float tanHalfAngle, out float occlusion) {
    float lod = 0;
    vec3 color = vec3(0);
    float alpha = 0;
    occlusion = 0;

    float dist = perVoxelSize;

    int count = 0;

    while (dist < MAX_DIST && alpha < ALPHA_THRESH && count < MAX_COUNT) {
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


uint pcg_hash(uint seed)
{
  uint state = seed * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

// Used to advance the PCG state.
uint rand_pcg(inout uint rng_state)
{
  uint state = rng_state;
  rng_state = rng_state * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

// Advances the prng state and returns the corresponding random float.
float rand(inout uint state)
{
  uint x = rand_pcg(state);
  state = x;
  return float(x)*uintBitsToFloat(0x2f800004u);
}

vec3 rand_vec3(inout uint seed)
{
    return vec3(rand(seed), rand(seed), rand(seed));
}
float rand(inout uint seed, float min, float max)
{
    return min + (max-min)*rand(seed);
}
vec3 rand_vec3(inout uint seed, float min, float max)
{
    return vec3(rand(seed, min, max), rand(seed, min, max), rand(seed, min, max));
}

vec3 random_in_unit_sphere(inout uint seed)
{
    while(true)
    {
        vec3 p = rand_vec3(seed, -1, 1);
        if (length(p) >= 1) continue;
        return p;
    }
}
vec3 random_unit_vector(inout uint seed)
{
    return normalize(random_in_unit_sphere(seed));
}