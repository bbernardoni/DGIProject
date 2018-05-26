#version 330 core

// adjacent triangles in and up to three quads out
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=12) out;

// the object space coordinates
in vec3 posObj[6];

// spine and texture coordinates
out vec4 spinePos;
out vec2 p0Coord;
out float len;

// half width of the lines
uniform float halfWidth = 8.0;
// a hack to properly z bias the lines
// we would need the perspective matrix to do this properly
// but this works as long as neither the near or far clipping planes are changed
uniform float proj23 = 20.0/-99.9;

// dot multipliers for homogeneous clippling
vec4 clips[6] = {vec4(1,0,0,1), vec4(-1,0,0,1), vec4(0,1,0,1), vec4(0,-1,0,1), vec4(0,0,1,1), vec4(0,0,-1,1)};

// outcode for Cohen-Sutherland algorithm
int outCode(vec4 v){
    int outcode = 0;
    for(int i=0; i<6; i++){
        if(dot(clips[i],v) < 0)
			outcode |= (1 << i);
    }
    return outcode;
}

// emit an edge at the given coordinates
void EmitEdge(vec4 P0, vec4 P1){
	// clip edge using Cohen-Sutherland algorithm
	int ocP0 = outCode(P0);
	int ocP1 = outCode(P1);
	if((ocP0 & ocP1) != 0) // trivial reject
		return;
	int ocMask = ocP0 | ocP1;
	if(ocMask != 0){ // clipping necessary
		float P0alpha = 0;
		float P1alpha = 1;
		for(int i=0; i<6; i++){
			if((ocMask & (1 << i)) != 0){
				// find intersection along clip plane
				float P0dot = dot(clips[i],P0);
				float P1dot = dot(clips[i],P1);
				float alpha = P0dot/(P0dot-P1dot);
		
				// adjust alphas
				if((ocP0 & (1 << i)) != 0){
					if(P0alpha < alpha) P0alpha = alpha;
				}else{
					if(P1alpha > alpha) P1alpha = alpha;
				}
		
				if(P0alpha > P1alpha){ // line invisible
					return;
				}
			}
		}
		// clip vertices
		vec4 oldP0 = P0;
		if(ocP0 != 0)
			P0 = (1-P0alpha)*P0 + P0alpha*P1;
		if(ocP1 != 0)
			P1 = (1-P1alpha)*oldP0 + P1alpha*P1;
	}

	// calculate the line offsets
	vec2 thickness = vec2(halfWidth) / vec2(800,600);
	vec2 lineDelta = P1.xy/P1.w - P0.xy/P0.w;
	vec2 lineFor = normalize(lineDelta);
	vec4 lineForOffset = vec4(thickness * lineFor, 0, 0);
	vec2 lineRight = vec2(-lineFor.y, lineFor.x);
	vec4 lineRightOffset = vec4(thickness * lineRight, 0, 0);
	// calculate the z bias (again a bit of a hack)
	vec4 zBias0 = vec4(0, 0, proj23*(1-P0.w/(P0.w+0.005)), 0);
	vec4 zBias1 = vec4(0, 0, proj23*(1-P1.w/(P1.w+0.005)), 0);
	
	// calculate final output variables
	len = length(lineDelta * vec2(800,600));
	spinePos = P0 - lineForOffset*P0.w + zBias0;
	p0Coord = vec2( halfWidth, -halfWidth);
	gl_Position = P0 + (-lineForOffset + lineRightOffset)*P0.w + zBias0; EmitVertex();
	p0Coord = vec2(-halfWidth, -halfWidth);
	gl_Position = P0 + (-lineForOffset - lineRightOffset)*P0.w + zBias0; EmitVertex();
	spinePos = P1 + lineForOffset*P1.w + zBias1;
	p0Coord = vec2( halfWidth, len+halfWidth);
	gl_Position = P1 + ( lineForOffset + lineRightOffset)*P1.w + zBias1; EmitVertex();
	p0Coord = vec2(-halfWidth, len+halfWidth);
	gl_Position = P1 + ( lineForOffset - lineRightOffset)*P1.w + zBias1; EmitVertex();
	EndPrimitive();
}

// determine if three points form a front facing triangle (ccw winding order)
bool IsFront(vec4 A, vec4 B, vec4 C){
    return 0 < (A.x*B.y - B.x*A.y)*C.w + (B.x*C.y - C.x*B.y)*A.w + (C.x*A.y - A.x*C.y)*B.w;
}

// get the normal of the triangle formed by three points
vec3 getNorm(vec3 A, vec3 B, vec3 C){
    return normalize(cross(B - A, C - A));
}

void main(){
	vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 v3 = gl_in[3].gl_Position;
    vec4 v4 = gl_in[4].gl_Position;
    vec4 v5 = gl_in[5].gl_Position;

	// only draw if front facing
    if(IsFront(v0, v2, v4)) {
		// calculate object space normals (must be done before the perspective matrix)
		vec3 norm0 = getNorm(posObj[0], posObj[2], posObj[4]);
		vec3 norm1 = getNorm(posObj[0], posObj[1], posObj[2]);
		vec3 norm2 = getNorm(posObj[2], posObj[3], posObj[4]);
		vec3 norm3 = getNorm(posObj[4], posObj[5], posObj[0]);

		// emit edge if it forms a silhouette or crease or undefined edge
        if(!IsFront(v0, v1, v2) || dot(norm0, norm1) < 0.5 || posObj[1].x > 100000.0)
			EmitEdge(v0, v2);
        if(!IsFront(v2, v3, v4) || dot(norm0, norm2) < 0.5 || posObj[3].x > 100000.0)
			EmitEdge(v2, v4);
        if(!IsFront(v4, v5, v0) || dot(norm0, norm3) < 0.5 || posObj[5].x > 100000.0)
			EmitEdge(v4, v0);
    }
}