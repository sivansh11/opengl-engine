#version 450 core

struct LightData {
    vec3 pos;
    vec3 color;
    vec3 term;
};

out vec4 frag;

uniform sampler2D gAlbedoSpec;
uniform sampler2D gPosition;
uniform sampler2D gNormal;

// layout (location = 0) in vec3 pos;
// layout (location = 1) in vec3 nor;
// layout (location = 2) in vec2 uv;
// layout (location = 3) in vec4 fragPosLightSpace;
// layout (location = 4) in mat3 TBN;

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

uniform sampler2D depthMap;
uniform vec3 viewPos;
uniform int numLights;

layout (std430, binding = 0) buffer LightDataBuffer {
    LightData lightDataBuffer[];
};

uniform struct DirectionalLight {
    vec3 pos;
    vec3 color;
    vec3 ambience;
} directionalLight;

vec3 calculateDirectionalLight();
vec3 calculatePointLight(int index);
float shadowCalculation();

void main() {
    vec3 accumulate = vec3(0, 0, 0);

    if (material.hasDiffuseMap) {
        if (texture(material.diffuseMap, uv).a == 0) {
            discard;
        }
    }
    
    accumulate += calculateDirectionalLight();

    for (int i = 0; i < numLights; i++) {
        accumulate += calculatePointLight(i);
    }
    
    frag = vec4(accumulate, 1);
}

float shadowCalculation() {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 normal = normalize(nor);
    vec3 lightDir = normalize(directionalLight.pos - pos);
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

vec3 calculatePointLight(int index) {
    LightData lightData = lightDataBuffer[index];

    vec3 norm;
    if (material.hasNormalMap) {
        norm = texture(material.normalMap, uv).xyz * 2 - 1;
    } else {
        norm = nor;
    }
    norm = normalize(TBN * norm);

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular;
    if (material.hasSpecularMap) {
        specular = lightData.color * spec * texture(material.specularMap, uv).r;
    } else {
        specular = lightData.color * spec * material.specular;
    }
    float distance = length(lightData.pos - pos);
    float attenuation = 1.0f / (lightData.term.r + lightData.term.g * distance + lightData.term.b * distance * distance);
    return diffuse * attenuation + specular * attenuation;
}

vec3 calculateDirectionalLight() {
    float shadow = shadowCalculation();

    vec3 ambient;
    if (material.hasDiffuseMap) {
        ambient = directionalLight.ambience * texture(material.diffuseMap, uv).rgb;
    } else {
        ambient = directionalLight.ambience * material.diffuse;
    }

    vec3 norm;
    if (material.hasNormalMap) {
        norm = texture(material.normalMap, uv).xyz * 2 - 1;
    } else {
        norm = nor;
    }
    norm = normalize(TBN * norm);

    vec3 lightDir = normalize(directionalLight.pos - pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse;
    if (material.hasDiffuseMap) {
        diffuse = directionalLight.color * diff * texture(material.diffuseMap, uv).rgb;
    } else {
        diffuse = directionalLight.color * diff * material.diffuse;
    }

    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular;
    if (material.hasSpecularMap) {
        specular = directionalLight.color * spec * texture(material.specularMap, uv).r;
    } else {
        specular = directionalLight.color * spec * material.specular;
    }
    return ambient + (diffuse + specular) * (1.0 - shadow);
}

