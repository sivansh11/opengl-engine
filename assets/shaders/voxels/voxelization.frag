#version 460 core
#extension GL_ARB_shader_image_load_store : enable

in fData {
    vec2 uv;
    mat4 projection;
    flat int axis;
    vec4 position;
} frag;

uniform layout(RGBA8) image3D voxels;

uniform struct Material {
    sampler2D diffuseMap;   
    sampler2D specularMap;
    sampler2D normalMap;
} material;

uniform sampler2DShadow depthMap;
uniform int voxelDim;

void main() {
    vec4 matColor = texture(material.diffuseMap, frag.uv);

    float visibility = texture(depthMap, vec3(frag.position.xy, (frag.position.z - 0.001) / frag.position.w));

    ivec3 camPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, voxelDim * gl_FragCoord.z);
    ivec3 texPos;
    if (frag.axis == 1) {
        texPos.x = voxelDim - camPos.z;
        texPos.z = camPos.x;
        texPos.y = camPos.y;

    } else if (frag.axis == 2) {
        texPos.z = camPos.y;
        texPos.y = voxelDim - camPos.z;
        texPos.x = camPos.x;
    } else {
        texPos = camPos;
    }

    texPos.z = voxelDim - texPos.z - 1;

    imageStore(voxels, texPos, vec4(matColor.rgb * visibility, 1.0));
}