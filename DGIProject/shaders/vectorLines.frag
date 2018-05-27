#version 330 core

// input data
in vec4 spinePos;
noperspective in vec2 p0Coord;
in float len;

// ouput data
out vec4 color;

// half width of the lines
uniform float halfWidth;
// depth texture
uniform sampler2DRect DepthSampler;

// custom depth test
// samples the spine pixel and the 8 adjacent pixels
bool depthTest(){
	for(float i=-1.0; i<1.1; i+=1.0){
		for(float j=-1.0; j<1.1; j+=1.0){
			vec2 texCoord = (spinePos.xy/spinePos.w+1)*0.5*vec2(800,600) + vec2(i,0) + vec2(0,j);
			float depth = texture(DepthSampler, texCoord).r;
			if(depth >= gl_FragCoord.z)
				return true;
		}
	}
	return false;
}

void main(){
	// discard if depth test fails
	if(!depthTest()){
		discard;
	}

	// calculate the pixel brightness based on distance to spine
    float fragFactor = 1.0 - abs(p0Coord.x)/halfWidth;
	if(p0Coord.y < 0)
		fragFactor = 1.0 - length(p0Coord)/halfWidth;
	else if(p0Coord.y > len)
		fragFactor = 1.0 - length(vec2(p0Coord.x, p0Coord.y-len))/halfWidth;

	// set ouput color
	vec3 rawColor = vec3(1.0);
	color = vec4(rawColor*fragFactor, 1.0);
}