#version 460 core

struct DirectionalLight {
    vec3 position;
    vec3 color;
    vec3 ambience;
    vec3 term;
};

in fData {
    vec2 uv;
} frag;

out vec4 outColor;

uniform sampler2DShadow shadowMap;
uniform DirectionalLight directionalLight;
uniform vec3 viewPos;
uniform mat4 invProjection;
uniform mat4 invView;
uniform mat4 lightSpace;
uniform float voxelGridSize;
uniform float tanHalfAngle;
uniform float ALPHA_THRESH;
uniform float MAX_DIST;
uniform int voxelDim;
uniform int samples;
uniform int MAX_COUNT;
uniform sampler2D texAlbedoSpec;
uniform sampler2D texDepth;
uniform sampler2D texNormal;
uniform sampler3D voxels;
uniform int outputType;

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

vec4 viewPosition;
vec4 worldPosition;
vec4 lightSpacePosition;
float perVoxelSize;
vec3 diffuseColor;
vec3 normal;
float specular;
float visibility;
vec3 startPos;

void main() {
    viewPosition = get_view_position_from_depth(frag.uv, texture(texDepth, frag.uv).r);
    worldPosition = invView * viewPosition;
    lightSpacePosition = lightSpace * worldPosition;
    perVoxelSize = voxelGridSize / voxelDim;
    normal = texture(texNormal, frag.uv).rgb;
    visibility = texture(shadowMap, vec3(lightSpacePosition.xy * .5 + .5, lightSpacePosition.z * .5 + .5 - 0.001 / lightSpacePosition.w));
    diffuseColor = texture(texAlbedoSpec, frag.uv).rgb;
    specular = texture(texAlbedoSpec, frag.uv).a;
    startPos = worldPosition.rgb + normal * perVoxelSize;

    uint pixel_index = uint(gl_FragCoord.x + gl_FragCoord.y * 10000);
    uint seed = pcg_hash(pixel_index);

    vec3 directLight = vec3(0);
    vec3 indirectLight = vec3(0);
    float occlusion = 0;
    
    // direct light
    {
        vec3 lightDir = normalize(directionalLight.position - worldPosition.rgb);
        vec3 diffuseComponent = directionalLight.color * max(dot(normal, lightDir), 0);

        vec3 viewDir = normalize(viewPos - worldPosition.rgb);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 specularComponent = directionalLight.color * pow(max(dot(viewDir, reflectDir), 0), 32) * specular; 

        vec3 ambientComponent = directionalLight.ambience;

        float distance = length(directionalLight.position - worldPosition.rgb);
        float attenuation = 1.0f / (directionalLight.term.r + directionalLight.term.g * distance + directionalLight.term.b * distance * distance);
        directLight = specularComponent * attenuation + diffuseComponent * attenuation;
    }

    // indirect light 
    {
        if (samples == 1) {
            indirectLight += coneTrace(startPos, normal, tanHalfAngle, occlusion).rgb;
        } else {
            for (int i = 0; i < samples; i++) {
                vec3 r = random_in_unit_sphere(seed);
                r /= 5;
                float tempOcclusion = 0;
                indirectLight += coneTrace(startPos, normalize(normal + r), tanHalfAngle, tempOcclusion).rgb;
                occlusion += tempOcclusion / samples;
            }
        }

        occlusion = 1 - occlusion;
        indirectLight /= samples;
    }

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
    const vec3 offset = vec3(1.0 / voxelDim, 1.0 / voxelDim, 0);
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