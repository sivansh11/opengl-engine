#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec3 in_worldPos[];
layout (location = 1) in vec3 in_nor[];

layout (location = 0) out vec3 out_worldPos;
layout (location = 1) out vec3 out_nor;

void main() {
    vec3 p1 = in_worldPos[1] - in_worldPos[0];
    vec3 p2 = in_worldPos[2] - in_worldPos[0];
    vec3 p = abs(cross(p1, p2));
    for (int i = 0; i < 3; i++) {
        out_worldPos = in_worldPos[i];
        out_nor = in_nor[i];
        if(p.z > p.x && p.z > p.y){
			gl_Position = vec4(out_worldPos.x, out_worldPos.y, 0, 1);
		} else if (p.x > p.y && p.x > p.z){
			gl_Position = vec4(out_worldPos.y, out_worldPos.z, 0, 1);
		} else {
			gl_Position = vec4(out_worldPos.x, out_worldPos.z, 0, 1);
		}
        EmitVertex();
    }
    EndPrimitive();
}