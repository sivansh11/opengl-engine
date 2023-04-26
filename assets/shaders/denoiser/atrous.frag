#version 460 core

#define PI 3.14159265

in fData {
    vec2 uv;
} frag;

out vec4 outColor;

uniform sampler2D texIndirectLight;
uniform sampler2D texNormal;
uniform sampler2D texDepth;

uniform mat4 invView;
uniform mat4 invProjection;

uniform float cPhi;
uniform float nPhi;
uniform float pPhi;
uniform int stepWidth;

uniform int width;
uniform int height;

const float kernel[25] = float[](
    1, 2, 3, 2, 1,
    2, 3, 5, 3, 2,
    3, 5, 7, 5, 3,
    2, 3, 5, 3, 2,
    1, 2, 3, 2, 1

    // 1, 1, 1,
    // 1, 1, 1,
    // 1, 1, 1
);

const ivec2 offset[25] = ivec2[](
    ivec2(-2, -2), ivec2(-2, -1), ivec2(-2,  0), ivec2(-2, 1), ivec2(-2, 2),
    ivec2(-1, -2), ivec2(-1, -1), ivec2(-1,  0), ivec2(-1, 1), ivec2(-1, 2),
    ivec2( 0, -2), ivec2( 0, -1), ivec2( 0,  0), ivec2( 0, 1), ivec2( 0, 2),
    ivec2( 1, -2), ivec2( 1, -1), ivec2( 1,  0), ivec2( 1, 1), ivec2( 1, 2),
    ivec2( 2, -2), ivec2( 2, -1), ivec2( 2,  0), ivec2( 2, 1), ivec2( 2, 2)

    // ivec2(-1, -1), ivec2(-1,  0), ivec2(-1,  1),
    // ivec2( 0, -1), ivec2( 0,  0), ivec2( 0,  1),
    // ivec2( 1, -1), ivec2( 1,  0), ivec2( 1,  1)
);

vec4 getViewPositionFromDepth(vec2 uv, float depth);

void main() {
    vec4 sum = vec4(0);
    vec2 step = vec2(1.0 / float(width), 1.0 / float(height));
    vec4 cVal = texture(texIndirectLight, frag.uv);
    vec3 nVal = texture(texNormal, frag.uv).rgb;
    vec3 pVal = (invView * getViewPositionFromDepth(frag.uv, texture(texDepth, frag.uv).r)).rgb;

    float cumW = 0;
    for (int i = 0; i < 25; i++) {
        vec2 uv = frag.uv + offset[i] * stepWidth * step;
        vec4 cTmp = texture(texIndirectLight, uv);
        vec4 cT = cVal - cTmp;
        float dist2 = dot(cT, cT);
        float cW = min(exp(-(dist2) / cPhi), 1.0);
        vec3 nTmp = texture(texNormal, frag.uv).rgb;
        vec3 nT = nVal - nTmp;
        dist2 = max(dot(nT, nT)/stepWidth * stepWidth, 0);
        float nW = min(exp(-(dist2) / nPhi), 1.0);

        vec3 pTmp = (invView * getViewPositionFromDepth(uv, texture(texDepth, uv).r)).rgb;
        vec3 pT = pVal - pTmp;
        dist2 = dot(pT, pT);
        float pW = min(exp(-(dist2) / pPhi), 1.0);
        float weight = cW * nW * pW;
        sum += cTmp * weight * kernel[i];
        cumW += weight * kernel[i];
    }

    outColor = sum / cumW;
}

vec4 getViewPositionFromDepth(vec2 uv, float depth) {
    float z = depth * 2 - 1;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition.xyzw;
}
