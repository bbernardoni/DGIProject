#version 330 core

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=12) out;

in vec3 posModel[6];

out vec2 spinePosL;
out vec2 spinePosC;
out vec2 spinePosR;
out vec3 dbg;

void EmitEdge(vec4 P0, vec4 P1){
	vec2 lineScreenForward = normalize(P1.xy/P1.w - P0.xy/P0.w);
    vec2 lineScreenRight = vec2(-lineScreenForward.y, lineScreenForward.x);
    vec4 lineScreenOffset = vec4(vec2(2.0) / vec2(800,600) * lineScreenRight, 0, 0);
	
	vec4 P0R = P0 + lineScreenOffset*P0.w;
	vec4 P0L = P0 - lineScreenOffset*P0.w;
	vec4 P1R = P1 + lineScreenOffset*P1.w;
	vec4 P1L = P1 - lineScreenOffset*P1.w;
	spinePosL = (P0L.xy/P0L.w + 1.0) * 0.5;
	spinePosC = (P0.xy /P0.w  + 1.0) * 0.5;
	spinePosR = (P0R.xy/P0R.w + 1.0) * 0.5;
    gl_Position = P0R; EmitVertex();
    gl_Position = P0L; EmitVertex();
	spinePosL = (P1L.xy/P1L.w + 1.0) * 0.5;
	spinePosC = (P1.xy /P1.w  + 1.0) * 0.5;
	spinePosR = (P1R.xy/P1R.w + 1.0) * 0.5;
    gl_Position = P1R; EmitVertex();
    gl_Position = P1L; EmitVertex();
    EndPrimitive();
}

bool IsFront(vec4 A, vec4 B, vec4 C){
    return 0 < (A.x*B.y - B.x*A.y)*C.w + (B.x*C.y - C.x*B.y)*A.w + (C.x*A.y - A.x*C.y)*B.w;
}

vec3 getNorm(vec3 A, vec3 B, vec3 C){
    return normalize(cross(B - A, C - A));
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
		vec3 norm0 = getNorm(posModel[0], posModel[2], posModel[4]);
		vec3 norm1 = getNorm(posModel[0], posModel[1], posModel[2]);
		vec3 norm2 = getNorm(posModel[2], posModel[3], posModel[4]);
		vec3 norm3 = getNorm(posModel[4], posModel[5], posModel[0]);
		dbg = vec3(dot(norm0, norm1), 1.0, 1.0);
        if(!IsFront(v0, v1, v2) || dot(norm0, norm1) < 0.5)
			EmitEdge(v0, v2);
		dbg = vec3(dot(norm0, norm2), 1.0, 1.0);
        if(!IsFront(v2, v3, v4) || dot(norm0, norm2) < 0.5)
			EmitEdge(v2, v4);
		dbg = vec3(dot(norm0, norm3), 1.0, 1.0);
        if(!IsFront(v4, v5, v0) || dot(norm0, norm3) < 0.5)
			EmitEdge(v4, v0);
    }
}