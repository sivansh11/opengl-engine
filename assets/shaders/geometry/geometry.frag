#version 460 core

in fData {
    vec3 normal;
    vec2 uv;
    vec3 T;
    vec3 B;
    vec3 N;
} frag;

uniform struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
} material;

out vec4 fragAlbedoSpec;
out vec4 fragNormal;

void main() {
    if (texture(material.diffuseMap, frag.uv).a == 0) {
        discard;
    }

    vec3 normal = texture(material.normalMap, frag.uv).xyz * 2 - 1;
    mat3 TBN = mat3(frag.T, frag.B, frag.N);
    normal = normalize(TBN * normal);
    fragNormal = vec4(normal, 0);
    fragAlbedoSpec.rgb = texture(material.diffuseMap, frag.uv).rgb;
    fragAlbedoSpec.a = texture(material.specularMap, frag.uv).r;
}
