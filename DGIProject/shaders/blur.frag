#version 330 core

// texture coordinates
in vec2 txCoord;

// ouput data
out vec4 color;

// texture sampler
uniform sampler2D image;
// direction of blur
uniform bool horizontal;

// offsets and weights for Gaussian blur
float offset[5] = {0.0, 1.469425947, 3.429053403, 5.389603405, 7.351547277};
float weight[5] = {0.113983509, 0.206238484, 0.138264212, 0.067308868, 0.023788921};

void main(){
     vec2 txOffset = 1.0 / textureSize(image, 0); // gets size of single texel

	 // perform blur
     vec3 result = texture(image, txCoord).rgb * weight[0];
     if(horizontal){
         for(int i = 1; i < 5; ++i){
            result += texture(image, txCoord + vec2(txOffset.x * offset[i], 0.0)).rgb * weight[i];
            result += texture(image, txCoord - vec2(txOffset.x * offset[i], 0.0)).rgb * weight[i];
         }
     }else{
         for(int i = 1; i < 5; ++i){
             result += texture(image, txCoord + vec2(0.0, txOffset.y * offset[i])).rgb * weight[i];
             result += texture(image, txCoord - vec2(0.0, txOffset.y * offset[i])).rgb * weight[i];
         }
     }
     color = vec4(result, 1.0);
}