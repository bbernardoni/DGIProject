#version 330 core

// texture coordinates
in vec2 txCoord;

// ouput data
out vec4 color;

// texture samplers
uniform sampler2D blur;
uniform sampler2D scene;
// bloom factor
uniform float bloomFactor;

void main(){
	// just add the color while multiplying bloom by a scalar
    vec3 bloomColor = texture(blur,  txCoord).rgb;
    vec3 sceneColor = texture(scene, txCoord).rgb;
    color = vec4(sceneColor + bloomColor*bloomFactor, 1.0);
}