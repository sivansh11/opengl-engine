#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 nor;
layout (location = 3) in mat3 TBN;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_nor;
layout (location = 2) out vec4 out_albedoSpec;

uniform struct Material {
    vec3 diffuse;
    float specular;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
} material;


void main() {
    out_pos = pos;

    if (material.hasDiffuseMap) 
        if (texture(material.diffuseMap, uv).a == 0)
            discard;

    if (material.hasNormalMap) {
        out_nor = texture(material.normalMap, uv).xyz * 2.0f - 1.f;
    } else {
        out_nor = nor;
    }
    out_nor = normalize(TBN * out_nor);

    if (material.hasDiffuseMap) {
        out_albedoSpec.rgb = texture(material.diffuseMap, uv).rgb * material.diffuse;
    } else {
        out_albedoSpec.rgb = material.diffuse;
    }
    if (material.hasSpecularMap) {
        out_albedoSpec.a = texture(material.specularMap, uv).a * material.specular;
    } else {
        out_albedoSpec.a = material.specular;
    }
}
