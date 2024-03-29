#version 460 core
#extension GL_ARB_shader_image_load_store : enable
#extension GL_NV_shader_atomic_fp16_vector : enable
#if defined GL_NV_shader_atomic_fp16_vector
#extension GL_NV_gpu_shader5 : require
#endif

struct DirectionalLight {
    vec3 position;
    vec3 color;
    vec3 ambience;
    vec3 term;
};

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 term;
};

uniform int numPointLights;
layout (std430, binding = 0) buffer PointLiDirectionalLightghtBuffer {
    PointLight pointLightBuffer[];
};

in fData {
    vec2 uv;
    mat4 projection;
    flat int axis;
    vec3 worldPosition;
    vec4 lightSpacePosition;
    vec3 T;
    vec3 B;
    vec3 N;
} frag;

uniform layout(rgba16f) image3D voxels;  // need to start using r32ui

uniform struct Material {
    sampler2D diffuseMap;   
    sampler2D specularMap;
    sampler2D normalMap;
    vec3 emmissive;
} material;

uniform sampler2DShadow texShadow;
uniform DirectionalLight directionalLight;

uniform int voxelDimensions;

vec3 calculateLight(int index);

mat3 TBN;
vec4 matColor;

void main() {
    matColor = texture(material.diffuseMap, frag.uv);
    if (matColor.a < 0.05) discard;

    TBN = mat3(frag.T, frag.B, frag.N);

    float visibility = texture(texShadow, vec3(frag.lightSpacePosition.xy, (frag.lightSpacePosition.z - 0.001) / frag.lightSpacePosition.w));

    ivec3 camPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, voxelDimensions * gl_FragCoord.z);
    ivec3 texPos;
    if (frag.axis == 1) {
        texPos.x = voxelDimensions - camPos.z;
        texPos.z = camPos.x;
        texPos.y = camPos.y;

    } else if (frag.axis == 2) {
        texPos.z = camPos.y;
        texPos.y = voxelDimensions - camPos.z;
        texPos.x = camPos.x;
    } else {
        texPos = camPos;
    }

    texPos.z = voxelDimensions - texPos.z - 1;

    vec3 color = vec3(0);
    
    float distance = length(directionalLight.position - frag.worldPosition);
    float attenuation = 1.0f / (directionalLight.term.r + directionalLight.term.g * distance + directionalLight.term.b * distance * distance);

    color = matColor.rgb * (visibility * directionalLight.color * attenuation + directionalLight.ambience);

    for (int i = 0; i < numPointLights; i++) {
        color += calculateLight(i);
    }

    color += material.emmissive * matColor.rgb;

    #if defined GL_NV_shader_atomic_fp16_vector
    imageAtomicMax(voxels, texPos, f16vec4(color, 1));
    #else
    imageStore(voxels, texPos, vec4(color, 1));
    #endif
}

vec3 calculateLight(int index) {
    PointLight pointLight = pointLightBuffer[index];

    vec3 norm = normalize(TBN * texture(material.normalMap, frag.uv).rgb);
    vec3 lightDir = normalize(pointLight.position - frag.worldPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.color * diff * matColor.rgb;
    
    float distance = length(pointLight.position - frag.worldPosition);
    float attenuation = 1.0f / (pointLight.term.r + pointLight.term.g * distance + pointLight.term.b * distance * distance);

    return diffuse * attenuation;
}