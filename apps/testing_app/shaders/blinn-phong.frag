#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 nor;
layout (location = 3) in mat3 TBN;

layout (location = 0) out vec4 frag;

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // vec3 term;
};

layout (std430, binding = 0) buffer PointLights {
    PointLight pointLights[];
};

uniform vec3 viewPos;

uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    float shininess;
} material;

void main() {
    vec3 accumulative = vec3(0, 0, 0);

    if (texture(material.diffuseMap, uv).a == 0) {
        discard;
    }

    for (int i = 0; i < pointLights.length(); i++) {
        PointLight p = pointLights[i];

        vec3 ambient;
        // if (material.hasDiffuseMap) {
            // ambient = p.ambient * material.ambient * texture(material.diffuseMap, uv).rgb;
            ambient = p.ambient * texture(material.diffuseMap, uv).rgb;
        // } else {
            // ambient = p.ambient * material.ambient;
        // }

        vec3 norm;
        // if (material.hasNormalMap) {
            norm = normalize(texture(material.normalMap, uv).xyz * 2.f - 1.f);
        // } 
        norm = normalize(TBN * norm);

        vec3 lightDir = normalize(p.pos - pos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse;
        // if (material.hasDiffuseMap) {
            // diffuse = p.diffuse * diff * material.diffuse * texture(material.diffuseMap, uv).rgb;
            diffuse = p.diffuse * diff * texture(material.diffuseMap, uv).rgb;
        // } else {
            // diffuse = p.diffuse * diff * material.diffuse;
        // }

        vec3 viewDir = normalize(viewPos - pos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular;
        // if (material.hasSpecularMap) {
            // specular = p.specular * spec * material.specular * texture(material.specularMap, uv).r;
            specular = p.specular * spec * texture(material.specularMap, uv).r;
        // } else {
            // specular = p.specular * spec * material.specular;
        // }

        accumulative += ambient + diffuse + specular;
    }

    frag = vec4(accumulative, 1);

}
