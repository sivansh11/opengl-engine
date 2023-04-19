#version 460 core

out vec4 color;

uniform int voxelDim;
uniform float voxelGridSize;
uniform sampler3D voxels;

void main() {
	vec3 pos; // Center of voxel
	pos.x = gl_VertexID % voxelDim;
	pos.z = (gl_VertexID / voxelDim) % voxelDim;
	pos.y = gl_VertexID / (voxelDim*voxelDim);

	color = texture(voxels, pos/voxelDim);
	gl_Position = vec4(pos - voxelDim*0.5, 1);
}