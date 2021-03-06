#version 330 core

// input vextex data
layout(location = 0) in vec3 pos;

// model view projection matrix
uniform mat4 MVP;

void main(){
	// output position in clip space
	gl_Position = MVP * vec4(pos,1);
}

