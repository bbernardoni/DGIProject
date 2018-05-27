#version 330 core

// texture coordinates
in vec2 txCoord;

// ouput data
out vec4 color;

// texture sampler
uniform sampler2D image;
// direction of blur
uniform bool horizontal;

float weight[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

void main(){
     vec2 txOffset = 1.0 / textureSize(image, 0); // gets size of single texel

	 // perform blur
     vec3 result = texture(image, txCoord).rgb * weight[0];
     if(horizontal){
         for(int i = 1; i < 5; ++i){
            result += texture(image, txCoord + vec2(txOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, txCoord - vec2(txOffset.x * i, 0.0)).rgb * weight[i];
         }
     }else{
         for(int i = 1; i < 5; ++i){
             result += texture(image, txCoord + vec2(0.0, txOffset.y * i)).rgb * weight[i];
             result += texture(image, txCoord - vec2(0.0, txOffset.y * i)).rgb * weight[i];
         }
     }
     color = vec4(result, 1.0);
}