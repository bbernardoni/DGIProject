#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

// Output data ; will be interpolated for each fragment.
out vec2 TexCoords;

void main(){
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}