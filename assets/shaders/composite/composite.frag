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


in fData {
    vec2 uv;
} frag;

out vec4 outColor;

uniform DirectionalLight directionalLight;
uniform sampler2D texAlbedoSpec;
uniform sampler2D texNormal;
uniform sampler2D texDepth;
uniform sampler2D texShadow;
uniform sampler2D texAmbientOcclusion;  // could be ssao, vxao, doesnt matter
uniform sampler2D texIndirectLight;  
uniform mat4 invProjection;
uniform mat4 invView;
uniform mat4 lightSpace;
uniform vec3 cameraPosition;
uniform int numPointLights;

layout (std430, binding = 0) buffer PointLiDirectionalLightghtBuffer {
    PointLight pointLightBuffer[];
};

vec4 getViewPositionFromDepth(vec2 uv, float depth);
vec3 getDirectionalLight();
vec3 getPointLight();
float shadowCalculation();

vec4 worldPosition;
vec4 viewPosition;
vec3 normal;
vec4 albedoSpec;
vec3 color;
float specular;
float ao;

void main() {
    viewPosition = getViewPositionFromDepth(frag.uv, texture(texDepth, frag.uv).r);
    worldPosition = invView * viewPosition;
    normal = texture(texNormal, frag.uv).rgb;
    albedoSpec = texture(texAlbedoSpec, frag.uv);
    color = albedoSpec.rgb;
    specular = albedoSpec.a;
    ao = texture(texAmbientOcclusion, frag.uv).r;

    vec3 directLight = vec3(0);

    directLight += (getDirectionalLight() + directionalLight.ambience * ao);
    directLight += getPointLight();

    vec3 indirectLight = texture(texIndirectLight, frag.uv).rgb;

    outColor = vec4((directLight + indirectLight) * color, 1);
}

vec4 getViewPositionFromDepth(vec2 uv, float depth) {
    float z = depth * 2 - 1;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition.xyzw;
}

float shadowCalculation() {
    vec4 fragPosLightSpace = lightSpace * worldPosition;   
    // return 1 - texture(texShadow, vec3(fragPosLightSpace.xy, (fragPosLightSpace.z - 0.001) / fragPosLightSpace.w));

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(texShadow, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(directionalLight.position - worldPosition.xyz);
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(texShadow, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(texShadow, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return 1 - shadow;
}

vec3 getDirectionalLight() {
    float shadow = shadowCalculation();

    vec3 lightDir = normalize(directionalLight.position - worldPosition.xyz);
    vec3 diffuseComponent = directionalLight.color * max(dot(normal, lightDir), 0.0);
    
    vec3 viewDir = normalize(cameraPosition - worldPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specularComponent = directionalLight.color * pow(max(dot(viewDir, reflectDir), 0), 32) * specular;

    float distance = length(directionalLight.position - worldPosition.xyz);
    float attenuation = 1.0f / (directionalLight.term.r + directionalLight.term.g * distance + directionalLight.term.b * distance * distance);
    
    return (diffuseComponent * attenuation + specularComponent * attenuation) * shadow;
}

vec3 getPointLight() {
    vec3 accumulator = vec3(0);
    for (int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLightBuffer[i].position - worldPosition.xyz);
        vec3 diffuseComponent = pointLightBuffer[i].color * max(dot(normal, lightDir), 0.0);

        vec3 viewDir = normalize(cameraPosition - worldPosition.xyz);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 specularComponent = pointLightBuffer[i].color * pow(max(dot(viewDir, reflectDir), 0), 32) * specular;

        float distance = length(pointLightBuffer[i].position - worldPosition.xyz);
        float attenuation = 1.0f / (pointLightBuffer[i].term.r + pointLightBuffer[i].term.g * distance + directionalLight.term.b * distance * distance);

        accumulator += diffuseComponent * attenuation + specularComponent * attenuation;
    }   

    
    return accumulator;
}

