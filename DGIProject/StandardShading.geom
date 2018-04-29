#version 330 core

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

vec3 coords[3] = vec3[](vec3(1,0,0),vec3(0,1,0),vec3(0,0,1));

in vec2 vUV[3];
 
out vec2 fUV;
out vec3 vBC;
 
void main(){
	for(int i = 0; i < 3; i++){
		// copy attributes
		gl_Position = gl_in[i].gl_Position;
		fUV = vUV[i];
		vBC = coords[i];

		// done with the vertex
		EmitVertex();
	}
}