#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 nor;
layout (location = 3) in mat3 TBN;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_nor;
layout (location = 2) out vec4 out_albedoSpec;

uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    bool hasDiffuseMap;
    bool hasNormalMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    float shininess;
} material;


void main() {
    out_pos = pos;

    if (material.hasNormalMap) {
        out_nor = texture(material.normalMap, uv).xyz * 2.0f - 1.f;
    }
    out_nor = normalize(TBN * out_nor);

    if (material.hasDiffuseMap) {
        out_albedoSpec.rgb = texture(material.diffuseMap, uv).rgb * material.diffuse;
    } else {
        out_albedoSpec.rgb = material.diffuse;
    }

    out_albedoSpec.a = texture(material.specularMap, uv).a;
}
