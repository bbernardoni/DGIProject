#version 330 core

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=12) out;

out vec2 spinePos;

void EmitEdge(vec4 P0, vec4 P1){
	vec2 lineScreenForward = normalize(P1.xy/P1.w - P0.xy/P0.w);
    vec2 lineScreenRight = vec2(-lineScreenForward.y, lineScreenForward.x);
    vec4 lineScreenOffset = vec4(vec2(3.0) / vec2(800,600) * lineScreenRight, 0, 0);
	
	vec4 P0r = P0 + lineScreenOffset*P0.w;
	vec4 P0l = P0 - lineScreenOffset*P0.w;
	vec4 P1r = P1 + lineScreenOffset*P1.w;
	vec4 P1l = P1 - lineScreenOffset*P1.w;
	spinePos = (P0l.xy/P0l.w + 1.0) * 0.5;
    gl_Position = P0r; EmitVertex();
    gl_Position = P0l; EmitVertex();
	spinePos = (P1l.xy/P1l.w + 1.0) * 0.5;
    gl_Position = P1r; EmitVertex();
    gl_Position = P1l; EmitVertex();
    EndPrimitive();
}

bool IsFront(vec4 A, vec4 B, vec4 C){
    return 0 < (A.x*B.y - B.x*A.y)*C.w + (B.x*C.y - C.x*B.y)*A.w + (C.x*A.y - A.x*C.y)*B.w;
}

void main(){
	/*for(int i = 0; i < 3; i++){
		// copy attributes
		gl_Position = gl_in[i].gl_Position;

		// done with the vertex
		EmitVertex();
	}
	EndPrimitive();*/

	vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 v3 = gl_in[3].gl_Position;
    vec4 v4 = gl_in[4].gl_Position;
    vec4 v5 = gl_in[5].gl_Position;
    
    if(IsFront(v0, v2, v4)) {
        if(!IsFront(v0, v1, v2)) EmitEdge(v0, v2);
        if(!IsFront(v2, v3, v4)) EmitEdge(v2, v4);
        if(!IsFront(v0, v4, v5)) EmitEdge(v4, v0);
    }
}