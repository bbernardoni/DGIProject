#version 330 core

// Interpolated values from the vertex shaders
in vec2 fUV;
in vec3 fPosition_worldspace;
in vec3 fNormal_cameraspace;
in vec3 fEyeDirection_cameraspace;
in vec3 fLightDirection_cameraspace;
in vec3 vBC;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, fUV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Distance to the light
	float distance = length( LightPosition_worldspace - fPosition_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( fNormal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( fLightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(fEyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	vec3 shadeColor = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	
	vec3 d = vBC/fwidth(vBC);
    //float minD = min(min(d.x, d.y), d.z);
	vec3 asdf = clamp(0.8 - d/2.0, 0, 0.8);
    float edgeFactor = asdf.x + asdf.y + asdf.z;//clamp(minD/3.0, 0, 1);
	color = vec4(MaterialDiffuseColor, edgeFactor);
}