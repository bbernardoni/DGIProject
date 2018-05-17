#version 330 core

// Interpolated values from the vertex shaders
in vec2 fUV;
in vec3 vBC;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){
	// Material properties
	vec3 texColor = texture(myTextureSampler, fUV).rgb;
	
	vec3 d = vBC/fwidth(vBC);
    float minD = min(min(d.x, d.y), d.z);

	//vec3 edgeFactor = clamp(0.8 - d/2.0, 0, 0.8);
    //float fragFactor = edgeFactor.x + edgeFactor.y + edgeFactor.z;
    float fragFactor = 1.0;// - smoothstep(0.5, 1.5, minD);

	color = vec4(texColor, fragFactor);
}