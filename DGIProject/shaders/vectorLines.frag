#version 330 core

// ouput data
out vec4 color;

// input data
in vec2 spinePos;
in vec2 p0Coord;
in float len;

// depth texture
uniform sampler2DRect DepthSampler;

// custom depth test
// samples the spine pixel and the 8 adjacent pixels
bool depthTest(){
	for(float i=-1.0; i<1.1; i+=1.0){
		for(float j=-1.0; j<1.1; j+=1.0){
			vec2 texCoord = spinePos*vec2(800,600) + vec2(i,0) + vec2(0,j);
			float depth = texture(DepthSampler, texCoord).r;
			if(depth >= gl_FragCoord.z)
				return true;
		}
	}
	return false;
}

float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main(){
	// discard if depth test fails
	if(!depthTest()){
		discard;
	}
	float depth = texture(DepthSampler, spinePos*vec2(800,600)).r;

	// calculate the pixel brightness based on distance to spine
    float fragFactor = 1.0 - abs(p0Coord.x)/8.0;
	if(p0Coord.y < 0)
		fragFactor = 1.0 - length(p0Coord)/8.0;
	else if(p0Coord.y > len)
		fragFactor = 1.0 - length(vec2(p0Coord.x, p0Coord.y-len))/8.0;

	// set ouput color
	vec3 rawColor = vec3(1.0);
	color = vec4(rawColor*fragFactor, 1.0);
}