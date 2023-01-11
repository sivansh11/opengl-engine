#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 fragPosLightSpace;
layout (location = 4) in mat3 TBN;

layout (location = 0) out vec4 out_albedoSpec;
layout (location = 1) out vec4 out_pos;
layout (location = 2) out vec4 out_nor;
 
uniform struct Material {
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    float specular;
    float shininess;
    vec3 diffuse;
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D specularMap;
} material;

void main() {
    if (material.hasDiffuseMap) {
        out_albedoSpec.rgb = texture(material.diffuseMap, uv).rgb;
        if (material.hasSpecularMap) {
            out_albedoSpec.a = texture(material.specularMap, uv).r;
        } else {
            out_albedoSpec.a = material.specular;
        }
    } else {
        out_albedoSpec.rgb = material.diffuse;
        if (material.hasSpecularMap) {
            out_albedoSpec.a = texture(material.specularMap, uv).r;
        } else {
            out_albedoSpec.a = material.specular;
        }
    }

    out_pos.rgb = pos;
    out_pos.a = material.shininess;

    if (material.hasNormalMap) {
        out_nor.rgb = texture(material.normalMap, uv).xyz * 2 - 1;
    } else {
        out_nor.rgb = nor;
    }
    out_nor.rgb = TBN * out_nor.rgb;
}

