#ifndef VBOINDEXER_HPP
#define VBOINDEXER_HPP

#include "objloader.h"

void indexVBO(
	std::vector<Vertex> & in_vertices,

	std::vector<unsigned short> & out_indices,
	std::vector<Vertex> & out_vertices
);

#endif