#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "vboindexer.h"

#include <string.h> // for memcmp


bool getSimilarVertexIndex( 
	Vertex & packed, 
	std::map<Vertex,unsigned short> & VertexToOutIndex,
	unsigned short & result
){
	std::map<Vertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() ){
		return false;
	}else{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<Vertex> & in_vertices,
	std::vector<unsigned short> & out_indices,
	std::vector<Vertex> & out_vertices
){
	std::map<Vertex,unsigned short> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){
		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(in_vertices[i], VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[in_vertices[i]] = newindex;
		}
	}
}
