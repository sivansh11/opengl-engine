#version 450 core

struct LightData {
    vec3 pos;
    vec3 color;
    vec3 term;
    vec3 ambience;
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 fragPosLightSpace;
layout (location = 4) in mat3 TBN;

out vec4 frag;

uniform struct Material {
    vec3 diffuse;
    float specular;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D specularMap;
} material;

uniform sampler2D depthMap;
uniform vec3 viewPos;

// layout (std430, binding = 0) buffer LightDataBuffer {
//     LightData lightDataBuffer[];
// };

uniform LightData lightData;

vec3 calculateLight(int index, float shadow);
float shadowCalculation();

void main() {
    vec3 accumulate = vec3(0, 0, 0);

    if (material.hasDiffuseMap) {
        if (texture(material.diffuseMap, uv).a == 0) {
            discard;
        }
    }
    
    float shadow = shadowCalculation();

    accumulate += calculateLight(0, shadow);

    // if (material.hasDiffuseMap) 
    //     accumulate = texture(material.diffuseMap, uv).rgb;
    // else 
    //     accumulate = material.diffuse;

    frag = vec4(accumulate, 1);
}

float shadowCalculation() {
    // vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // projCoords = projCoords * 0.5 + 0.5; 
    // float closestDepth = texture(depthMap, projCoords.xy).r;   
    // float currentDepth = projCoords.z;  
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;  
    // return shadow;

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(nor);
    vec3 lightDir = normalize(lightData.pos - pos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
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
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 calculateLight(int index, float shadow) {

    vec3 ambient;
    if (material.hasDiffuseMap) {
        ambient = lightData.ambience * texture(material.diffuseMap, uv).rgb;
    } else {
        ambient = lightData.ambience * material.diffuse;
    }

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular;
    if (material.hasSpecularMap) {
        specular = lightData.color * spec * texture(material.specularMap, uv).r;
    } else {
        specular = lightData.color * spec * material.specular;
    }

    float distance = length(lightData.pos - pos);
    float attenuation = 1.0f / (lightData.term.r + lightData.term.g * distance + lightData.term.b * distance * distance);

    return ambient + (diffuse * attenuation + specular * attenuation) * (1.0 - shadow);
}

