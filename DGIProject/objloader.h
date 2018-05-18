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

bool loadAssImp(
	const char * path,
	std::vector<unsigned short> & out_indices,
	std::vector<Vertex> & out_vertices
);

bool loadOBJ(
	const char * path,
	std::vector<Vertex> & out_vertices
);

bool loadVertOBJ(
	const char * path,
	std::vector<unsigned short> & out_indices,
	std::vector<Vertex> & out_vertices
);

void indexVBO(
	std::vector<Vertex> & in_vertices,

	std::vector<unsigned short> & out_indices,
	std::vector<Vertex> & out_vertices
);

void genTrianglesAdjacency(
	std::vector<unsigned short> & in_indices,
	std::vector<unsigned short> & out_indices
);

#endif