#version 460 core

layout (triangles) in; // glDrawArrays is set to triangles so that's what we're working with
layout (triangle_strip, max_vertices = 3) out;

in vData {
    vec2 uv;
    vec4 lightSpacePosition;
    vec3 worldPosition;
    vec3 T;
    vec3 B;
    vec3 N;
} verts[];

out fData {
    vec2 uv;
    mat4 projection;
    flat int axis;
    vec3 worldPosition;
    vec4 lightSpacePosition;
    vec3 T;
    vec3 B;
    vec3 N;
} frag;

uniform mat4 projX;
uniform mat4 projY;
uniform mat4 projZ;

void main() {
    vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(p1,p2));

    float dotX = abs(normal.x);
    float dotY = abs(normal.y);
    float dotZ = abs(normal.z);

    frag.axis = (dotX >= dotY && dotX >= dotZ) ? 1 : (dotY >= dotX && dotY >= dotZ) ? 2 : 3;
    frag.projection = frag.axis == 1 ? projX : frag.axis == 2 ? projY : projZ;

    for (int i = 0; i < gl_in.length(); i++) {
        frag.uv = verts[i].uv;
        frag.lightSpacePosition = verts[i].lightSpacePosition;
        frag.worldPosition = verts[i].worldPosition;
        frag.T = verts[i].T;
        frag.B = verts[i].B;
        frag.N = verts[i].N;
        gl_Position = frag.projection * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}


