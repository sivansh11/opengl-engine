#version 450 core

struct LightData {
    vec3 pos;
    vec3 color;
    vec3 term;
    vec3 ambient;
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;

out vec4 frag;

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

uniform vec3 viewPos;
uniform int numLights;

layout (std430, binding = 0) buffer LightDataBuffer {
    LightData lightDataBuffer[];
};

vec3 calculateLight(int index);

void main() {
    if (material.hasDiffuseMap) {
        if (texture(material.diffuseMap, uv).a == 0) {
            discard;
        }
    }

    vec3 accumulate = vec3(0, 0, 0);

    for (int i = 0; i < numLights; i++) {
        accumulate += calculateLight(i);
    }

    frag = vec4(accumulate, 1);
}

vec3 calculateLight(int index) {
    LightData lightData = lightDataBuffer[index];

    vec3 ambient;
    if (material.hasDiffuseMap) {
        ambient = lightData.ambient * texture(material.diffuseMap, uv).rgb;
    } else {
        ambient = lightData.ambient * material.diffuse;
    }

    vec3 norm;
    if (material.hasNormalMap) {
        norm = normalize(texture(material.normalMap, uv).xyz * 2.f - 1.f);
    } else {
        norm = normalize(nor);
    }

    vec3 lightDir = normalize(lightData.pos - pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse;
    if (material.hasDiffuseMap) {
        diffuse = lightData.color * diff * texture(material.diffuseMap, uv).rgb;
    } else {
        diffuse = lightData.color * diff * material.diffuse;
    }

    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular;
    if (material.hasSpecularMap) {
        specular = lightData.color * spec * texture(material.specularMap, uv).r;
    } else {
        specular = lightData.color * spec * material.specular;
    }

    float distance = length(lightData.pos - pos);
    float attenuation = 1.0f / (lightData.term.r + lightData.term.g * distance + lightData.term.b * distance * distance);

    return ambient + diffuse * attenuation + specular * attenuation;
}

