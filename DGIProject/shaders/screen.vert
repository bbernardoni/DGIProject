#version 330 core

// input vextex data
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

// texture coordinates
out vec2 txCoord;

void main(){
    gl_Position = vec4(pos, 1.0);
    txCoord = uv;
}