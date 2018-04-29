#ifndef OBJLOADER_H
#define OBJLOADER_H

struct Vertex{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 norm;
	bool operator<(const Vertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(Vertex))>0;
	};
};

bool loadOBJ(
	const char * path, 
	std::vector<Vertex> & out_vertices
);

#endif