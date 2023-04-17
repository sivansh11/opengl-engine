#version 460 core

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 term;
};

struct DirectionalLight {
    vec3 position;
    vec3 color;
    vec3 ambience;
    vec3 term;
};

layout (location = 0) in vec2 uv;

out vec4 frag;

uniform sampler2D texAlbedoSpec;
uniform sampler2D texDepth;
uniform sampler2D texNormal;

uniform bool useSSAO;
uniform sampler2D texSSAO;

uniform sampler2D depthMap;
uniform DirectionalLight directionalLight;
uniform bool hasDirectionalLight;

uniform mat4 lightSpaceMatrix;

uniform mat4 invView;
uniform mat4 invProjection;

uniform vec3 viewPos;
uniform int numLights;

layout (std430, binding = 0) buffer PointLiDirectionalLightghtBuffer {
    PointLight pointLightBuffer[];
};

vec3 calculateDirectionalLight();
float shadowCalculation();
vec3 calculateLight(int index);

vec3 position;
vec3 normal;
vec3 color;
float specular;

vec4 get_view_position_from_depth(vec2 uv, float depth) {
    float z = depth * 2 - 1;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyzw;
}

void main() {
    position = vec3(invView * get_view_position_from_depth(uv, texture(texDepth, uv).r));
    normal = normalize(texture(texNormal, uv).rgb);
    color = texture(texAlbedoSpec, uv).rgb;
    specular = texture(texAlbedoSpec, uv).a;

    vec3 finalColor = vec3(0, 0, 0);
    for (int i = 0; i < numLights; i++) {
        finalColor += calculateLight(i);
    }

    if (hasDirectionalLight) {
        finalColor += calculateDirectionalLight();
    }

    frag = vec4(finalColor, 1);
}

vec3 calculateLight(int index) {
    PointLight pointLight = pointLightBuffer[index];

    vec3 norm = normal;

    vec3 lightDir = normalize(pointLight.position - position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.color * diff * color;
    
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0), 32);
    vec3 specular = pointLight.color * spec * specular;

    float distance = length(pointLight.position - position);
    float attenuation = 1.0f / (pointLight.term.r + pointLight.term.g * distance + pointLight.term.b * distance * distance);

    return diffuse * attenuation + specular * attenuation;
}

float shadowCalculation() {
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(position, 1);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(directionalLight.position - position);
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x) {
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

vec3 calculateDirectionalLight() {
    float shadow = shadowCalculation();

    vec3 norm = normal;

    vec3 lightDir = normalize(directionalLight.position - position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = directionalLight.color * diff * color;
    
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0), 32);
    vec3 specular = directionalLight.color * spec * specular;

    float distance = length(directionalLight.position - position);
    float attenuation = 1.0f / (directionalLight.term.r + directionalLight.term.g * distance + directionalLight.term.b * distance * distance);
    
    if (useSSAO) {
        return (directionalLight.ambience * color * texture(texSSAO, uv).r) + ((diffuse * attenuation + specular * attenuation) * (1.0 - shadow));
    } else {
        return (directionalLight.ambience * color) + ((diffuse * attenuation + specular * attenuation) * (1.0 - shadow));
    }
}

