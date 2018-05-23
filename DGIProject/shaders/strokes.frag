#version 330 core

// Ouput data
out vec4 color;

in vec2 spinePos;
in vec2 p0Coord;
in float len;

// Values that stay constant for the whole mesh.
uniform sampler2D DepthSampler;

bool depthTest(){
	for(float i=-1.0; i<1.1; i+=1.0){
		for(float j=-1.0; j<1.1; j+=1.0){
			vec2 texCoord = spinePos + i*vec2(1/800.0, 0.0) + j*vec2(0.0, 1/600.0);
			float depth = texture(DepthSampler, texCoord).r;
			if(depth >= gl_FragCoord.z)
				return true;
		}
	}
	return false;
}

void main(){
	if(!depthTest())
		discard;

    float fragFactor = 1.0 - abs(p0Coord.x)/8.0;
	if(p0Coord.y < 0)
		fragFactor = 1.0 - length(p0Coord)/8.0;
	else if(p0Coord.y > len)
		fragFactor = 1.0 - length(vec2(p0Coord.x, p0Coord.y-len))/8.0;

	color = vec4(fragFactor, fragFactor, fragFactor, 1.0);
}