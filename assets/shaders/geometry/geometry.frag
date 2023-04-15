#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat3 TBN;

uniform struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
} material;

out vec4 fragAlbedoSpec;
out vec4 fragNormal;
out vec4 fragPosition;

void main() {
    if (texture(material.diffuseMap, uv).a == 0) {
        discard;
    }

    fragPosition = vec4(position, 1);

    vec3 norm = texture(material.normalMap, uv).xyz * 2 - 1;
    norm = normalize(TBN * norm);
    fragNormal = vec4(norm, 0);

    fragAlbedoSpec.rgb = texture(material.diffuseMap, uv).rgb;

    fragAlbedoSpec.a = texture(material.specularMap, uv).r;
}
