#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 nor;
layout (location = 3) in mat3 TBN;

layout (location = 0) out vec4 frag;

struct PointLight {
    vec3 pos;
    vec3 color;
    vec3 term;
    vec3 ambient;
};
layout (std430, binding = 0) buffer PointLights {
    PointLight pointLights[];
};

uniform vec3 viewPos;

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

uniform int numLights;

void main() {
    vec3 accumulative = vec3(0, 0, 0);

    if (texture(material.diffuseMap, uv).a == 0) {
        discard;
    }

    for (int i = 0; i < numLights; i++) {
        PointLight p = pointLights[i];

        vec3 ambient;
        if (material.hasDiffuseMap) {
            ambient = p.ambient * texture(material.diffuseMap, uv).rgb;
        } else {
            ambient = p.ambient * material.diffuse;
        }

        vec3 norm;
        if (material.hasNormalMap) {
            norm = normalize(texture(material.normalMap, uv).xyz * 2.f - 1.f);
        } else {
            norm = normalize(nor);
        }
        norm = normalize(TBN * norm);

        vec3 lightDir = normalize(p.pos - pos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse;
        diffuse = p.color * diff * texture(material.diffuseMap, uv).rgb;

        vec3 viewDir = normalize(viewPos - pos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular;
        if (material.hasSpecularMap) {
            specular = p.color * spec * material.specular * texture(material.specularMap, uv).r;
        } else {
            specular = p.color * spec * material.specular;
        }

        accumulative += ambient + diffuse + specular;
    }

    frag = vec4(accumulative, 1);

}
