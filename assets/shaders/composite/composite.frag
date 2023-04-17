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
uniform sampler2D texPosition;
uniform sampler2D texNormal;

uniform vec3 viewPos;
uniform int numLights;

layout (std430, binding = 0) buffer PointLightBuffer {
    PointLight pointLightBuffer[];
};

// float shadowCalculation();
vec3 calculateLight(int index);

vec3 position;
vec3 normal;
vec3 color;
float specular;

void main() {
    position = texture(texPosition, uv).rgb;
    normal = texture(texNormal, uv).rgb;
    color = texture(texAlbedoSpec, uv).rgb;
    specular = texture(texAlbedoSpec, uv).a;

    frag = vec4(0, 0, 0, 1);
    for (int i = 0; i < numLights; i++) {
        frag += vec4(calculateLight(i), 1);
    }
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
