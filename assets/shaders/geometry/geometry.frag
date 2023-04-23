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
out vec4 fragTangent;

void main() {
    if (texture(material.diffuseMap, frag.uv).a == 0) {
        discard;
    }

    vec3 norm = texture(material.normalMap, frag.uv).xyz * 2 - 1;
    mat3 TBN = mat3(frag.T, frag.B, frag.N);
    norm = normalize(TBN * norm);
    fragNormal = vec4(norm, 0);
    // fragNormal = vec4(frag.normal, 0);

    fragAlbedoSpec.rgb = texture(material.diffuseMap, frag.uv).rgb;

    fragAlbedoSpec.a = texture(material.specularMap, frag.uv).r;

    fragTangent = vec4(frag.T, 0);
}
