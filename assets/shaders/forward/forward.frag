#version 460 core

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 term;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat3 TBN;

out vec4 frag;

uniform struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
} material;

uniform vec3 viewPos;
uniform int numLights;

layout (std430, binding = 0) buffer PointLightBuffer {
    PointLight pointLightBuffer[];
};

vec3 calculateLight(int index);

void main() {
    if (texture(material.diffuseMap, uv).a == 0) {
        discard;
    }
    vec3 color = vec3(0, 0, 0);

    for (int i = 0; i < numLights; i++) {
        color += calculateLight(i);
    }

    frag = vec4(color, 1);
}

vec3 calculateLight(int index) {
    PointLight pointLight = pointLightBuffer[index];

    vec3 norm = texture(material.normalMap, uv).xyz * 2 - 1;
    norm = normalize(TBN * norm);

    vec3 lightDir = normalize(pointLight.position - position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.color * diff * texture(material.diffuseMap, uv).rgb;
    
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0), 32);
    vec3 specular = pointLight.color * spec * texture(material.specularMap, uv).r;

    float distance = length(pointLight.position - position);
    float attenuation = 1.0f / (pointLight.term.r + pointLight.term.g * distance + pointLight.term.b * distance * distance);

    return diffuse * attenuation + specular * attenuation;
}