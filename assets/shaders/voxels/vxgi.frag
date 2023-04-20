#version 460 core

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 term;
};

struct DirectionalLight {
    vec3 position;
    vec3 color;
    vec3 ambience;
    vec3 term;
};

in fData {
    vec2 uv;
    vec3 T;
    vec3 B;
    vec3 N;
    vec3 worldPosition;
    vec4 lightSpacePosition;
} frag;

out vec4 outColor;

uniform struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
} material;

uniform sampler2DShadow depthMap;
uniform DirectionalLight directionalLight;
uniform bool hasDirectionalLight;

uniform vec3 viewPos;
uniform int numLights;

uniform int voxelDim;
uniform float voxelGridSize;
uniform sampler3D voxels;

uniform sampler2D texNoise;

layout (std430, binding = 0) buffer PointLiDirectionalLightghtBuffer {
    PointLight pointLightBuffer[];
};

vec4 sampleVoxel(vec3 worldPosition, float lod);
vec4 coneTrace(vec3 startPos, vec3 direction, float tanHalfAngle, out float occlusion);

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

uniform int samples;
uniform float tanHalfAngle;

float visibility;
vec3 diffuseColor;
mat3 TBN;
float perVoxelSize;

uniform int outputType;

void main() {
    if (texture(material.diffuseMap, frag.uv).a < 0.05) discard;

    uint pixel_index = uint(gl_FragCoord.x + gl_FragCoord.y * 10000);
    uint seed = pcg_hash(pixel_index);
    
    visibility = texture(depthMap, vec3(frag.lightSpacePosition.xy * .5 + .5, frag.lightSpacePosition.z * .5 + .5 - 0.001 / frag.lightSpacePosition.w));
    TBN = mat3(frag.T, frag.B, frag.N);
    diffuseColor = texture(material.diffuseMap, frag.uv).rgb;
    perVoxelSize = voxelGridSize / voxelDim;
    
    float occlusion = 0;
    vec3 indirectLight = vec3(0);

    vec3 startPos = frag.worldPosition + frag.N * perVoxelSize;

    if (samples != 1) {
        for (int i = 0; i < samples; i++) {
            vec3 r = random_in_unit_sphere(seed);
            r /= 1;
            float oc;
            indirectLight += coneTrace(startPos, TBN * (texture(material.normalMap, frag.uv).rgb + r), tanHalfAngle, oc).rgb;
            occlusion += oc / samples;
        }
    } else {
        float oc;
        indirectLight += coneTrace(startPos, TBN * texture(material.normalMap, frag.uv).rgb, tanHalfAngle, oc).rgb;
        occlusion += oc / samples;
    }
    
    // coneTrace(startPos, TBN * texture(material.normalMap, frag.uv).rgb, .577, occlusion);

    occlusion = 1 - occlusion;  
    
    indirectLight /= samples;

    // indirectLight *= occlusion;

    vec3 directLight = vec3(0);
    vec3 normal = texture(material.normalMap, frag.uv).rgb;
    normal = normalize(TBN * normal);

    vec3 lightDir = normalize(directionalLight.position - frag.worldPosition);
    vec3 diffuse = directionalLight.color * max(dot(normal, lightDir), 0);

    vec3 viewDir = normalize(viewPos - frag.worldPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float oc;
    vec3 specular = directionalLight.color * pow(max(dot(viewDir, reflectDir), 0), 32) * texture(material.specularMap, frag.uv).r * coneTrace(startPos, reflectDir, 0.07, oc).rgb;
    
    vec3 ambient = directionalLight.ambience;
    
    float distance = length(directionalLight.position - frag.worldPosition);
    float attenuation = 1.0f / (directionalLight.term.r + directionalLight.term.g * distance + directionalLight.term.b * distance * distance);

    directLight = specular * attenuation + diffuse * attenuation;
    
    if (outputType == 0)
        outColor = vec4((directLight * visibility + indirectLight * occlusion + directionalLight.ambience * occlusion) * diffuseColor.rgb, 1);
    if (outputType == 1)
        outColor = vec4(indirectLight * diffuseColor.rgb, 1);
    if (outputType == 2)
        outColor = vec4(indirectLight * occlusion * diffuseColor.rgb, 1);
    if (outputType == 3) 
        outColor = vec4(directLight * visibility * diffuseColor.rgb, 1);
    if (outputType == 4)
        outColor = vec4(occlusion, occlusion, occlusion, 1);
    if (outputType == 5) 
        outColor = vec4((directLight * visibility + directionalLight.ambience * occlusion) * diffuseColor.rgb, 1);
}

uniform float ALPHA_THRESH;
uniform float MAX_DIST;
uniform int MAX_COUNT;

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
    const vec3 offset = vec3(1.0 / voxelDim, 1.0 / voxelDim, 0);
    vec3 voxelTextureUV = worldPosition / (voxelGridSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5 + offset;
    return textureLod(voxels, voxelTextureUV, lod);
}