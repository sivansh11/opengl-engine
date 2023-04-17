#version 460 core

layout (location = 0) in vec2 uv;

out float frag;

uniform sampler2D texPosition;
uniform sampler2D texNormal;
uniform sampler2D texNoise;

uniform float radius;
uniform float bias;
uniform mat4 projection;
uniform mat4 view;
uniform int numSamples;

layout (std430, binding = 0) buffer SSAOKernelBuffer {
    vec4 samples[];
};

void main() {
    vec3 fragPos = vec3(view * vec4(texture(texPosition, uv).xyz, 1));
    vec3 fragNormal = mat3(view) * texture(texNormal, uv).xyz;
    vec3 randVec3 = normalize(texture(texNoise, uv).xyz);

    vec3 tangent = normalize(randVec3 - fragNormal * dot(randVec3, fragNormal));
    vec3 biTangent = cross(fragNormal, tangent);
    mat3 TBN = mat3(tangent, biTangent, fragNormal);

    float occlusion = 0.0;
    for (int i = 0; i < samples.length(); i++) {
        vec3 samplePos = TBN * vec3(samples[i]);
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1);
        offset = projection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        float sampleDepth = (view * vec4(texture(texPosition, offset.xy).xyz, 1)).z;

        float rangeCheck = smoothstep(0, 1, radius / abs(fragPos.z - sampleDepth));

        occlusion += (sampleDepth > samplePos.z + bias ? 1 : 0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(samples.length()));
    frag = occlusion;
}
