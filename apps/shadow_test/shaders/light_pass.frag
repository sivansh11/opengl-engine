#version 450 core

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 frag;

uniform struct GBuffer {
    sampler2D posBuffer;
    sampler2D norBuffer;
    sampler2D albedoSpecBuffer;
} gBuffer;

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
uniform int numLights;

void main() {
    vec3 accumulative = vec3(0, 0, 0);

    for (int i = 0; i < numLights; i++) {
        PointLight p = pointLights[i];
        
        vec3 ambient = p.ambient * texture(gBuffer.albedoSpecBuffer, uv).rgb;

        vec3 lightDir = normalize(p.pos - texture(gBuffer.posBuffer, uv).xyz);
        float diff = max(dot(texture(gBuffer.norBuffer, uv).xyz, lightDir), 0.0);
        vec3 diffuse = p.color * diff * texture(gBuffer.albedoSpecBuffer, uv).rgb;

        vec3 viewDir = normalize(viewPos - texture(gBuffer.posBuffer, uv).xyz);
        vec3 reflectDir = reflect(-lightDir, texture(gBuffer.norBuffer, uv).xyz);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = p.color * spec * texture(gBuffer.albedoSpecBuffer, uv).a;

        float distance = length(p.pos - texture(gBuffer.posBuffer, uv).xyz);
        float attenuation = 1.0f / (p.term.r + p.term.g * distance + p.term.b * distance * distance);

        accumulative += ambient + diffuse * attenuation + specular * attenuation;
    }

    frag = vec4(accumulative, 1);
}