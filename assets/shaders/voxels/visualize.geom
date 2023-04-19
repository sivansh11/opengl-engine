#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

uniform int voxelDim;
uniform float voxelGridSize;
uniform sampler3D voxels;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

in vec4 color[];

out fData {
    vec4 color;
    vec3 normal;
} frag;

void main() {
    frag.color = color[0];

    vec4 v1 = projection * view * model * (gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.5, 0));  
    vec4 v2 = projection * view * model * (gl_in[0].gl_Position + vec4(0.5, 0.5, 0.5, 0));   
    vec4 v3 = projection * view * model * (gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.5, 0)); 
    vec4 v4 = projection * view * model * (gl_in[0].gl_Position + vec4(0.5, -0.5, 0.5, 0));  
    vec4 v5 = projection * view * model * (gl_in[0].gl_Position + vec4(-0.5, 0.5, -0.5, 0)); 
    vec4 v6 = projection * view * model * (gl_in[0].gl_Position + vec4(0.5, 0.5, -0.5, 0));  
    vec4 v7 = projection * view * model * (gl_in[0].gl_Position + vec4(-0.5, -0.5, -0.5, 0));
    vec4 v8 = projection * view * model * (gl_in[0].gl_Position + vec4(0.5, -0.5, -0.5, 0)); 

    // +Z
    frag.normal = vec3(0, 0, 1);
    gl_Position = v1;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
    gl_Position = v1;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();

    // -Z
    frag.normal = vec3(0, 0, -1);
    gl_Position = v6;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v7;
    EmitVertex();
    EndPrimitive();
    gl_Position = v6;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v5;
    EmitVertex();
    EndPrimitive();

    // +X
    frag.normal = vec3(1, 0, 0);
    gl_Position = v2;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v2;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -X
    frag.normal = vec3(-1, 0, 0);
    gl_Position = v5;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v3;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v1;
    EmitVertex();
    EndPrimitive();

    // +Y
    frag.normal = vec3(0, 1, 0);
    gl_Position = v5;
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -Y
    frag.normal = vec3(0, -1, 0);
    gl_Position = v3;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v3;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
}

