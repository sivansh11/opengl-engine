#version 460 core

in fData {
    vec2 uv;
} frag;

out float outAmbientOcclusion;

uniform sampler2D texDepth;
uniform sampler2D texNormal;
uniform sampler2D texNoise;
uniform float radius;
uniform float bias;
uniform mat4 projection;
uniform mat4 invProjection;
uniform mat4 view;
uniform mat4 invView;
uniform int numSamples;

layout (std430, binding = 1) buffer SSAOKernelBuffer {
    vec4 samples[];
};

vec4 getViewPositionFromDepth(vec2 uv, float depth);

void main() {
    vec3 worldPosition = vec3(getViewPositionFromDepth(frag.uv, texture(texDepth, frag.uv).r));
    vec3 normal = mat3(view) * texture(texNormal, frag.uv).xyz;
    vec3 r = normalize(texture(texNoise, frag.uv).xyz);

    vec3 tangent = normalize(r - normal * dot(r, normal));
    vec3 biTangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, biTangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < numSamples; i++) {
        vec3 samplePos = TBN * vec3(samples[i]);
        samplePos = worldPosition + samplePos * radius;

        vec4 offset = vec4(samplePos, 1);
        offset = projection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        float sampleDepth = getViewPositionFromDepth(offset.xy, texture(texDepth, offset.xy).r).z;

        float rangeCheck = smoothstep(0, 1, radius / abs(worldPosition.z - sampleDepth));

        occlusion += (sampleDepth > samplePos.z + bias ? 1 : 0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(numSamples));
    outAmbientOcclusion = occlusion;
}

vec4 getViewPositionFromDepth(vec2 uv, float depth) {
    float z = depth * 2 - 1;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition.xyzw;
}