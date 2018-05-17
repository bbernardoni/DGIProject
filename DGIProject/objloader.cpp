#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <map>

#include <glm/glm.hpp>

#include "objloader.h"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadOBJ(const char * path, std::vector<Vertex> & out_vertices){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE * file = fopen(path, "r");
	if(file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while(true){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		Vertex vert;
		vert.pos = temp_vertices[ vertexIndex-1 ];
		vert.uv = temp_uvs[ uvIndex-1 ];
		vert.norm = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vert);
	
	}
	fclose(file);
	return true;
}


bool loadVertOBJ(const char * path,	std::vector<unsigned short> & out_indices, std::vector<Vertex> & out_vertices){
	printf("Loading OBJ file %s...\n", path);

	FILE * file = fopen(path, "r");
	if(file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while(true){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if(strcmp(lineHeader, "v") == 0){
			Vertex vertex;
			fscanf(file, "%f %f %f\n", &vertex.pos.x, &vertex.pos.y, &vertex.pos.z);
			out_vertices.push_back(vertex);
		}
		else if(strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if(matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			out_indices.push_back(vertexIndex[0]-1);
			out_indices.push_back(vertexIndex[1]-1);
			out_indices.push_back(vertexIndex[2]-1);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	fclose(file);
	return true;
}

bool getSimilarVertexIndex(
	Vertex & packed,
	std::map<Vertex, unsigned short> & VertexToOutIndex,
	unsigned short & result
){
	std::map<Vertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if(it == VertexToOutIndex.end()){
		return false;
	}
	else{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<Vertex> & in_vertices,
	std::vector<unsigned short> & out_indices,
	std::vector<Vertex> & out_vertices
){
	std::map<Vertex, unsigned short> VertexToOutIndex;

	// For each input vertex
	for(unsigned int i=0; i<in_vertices.size(); i++){
		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(in_vertices[i], VertexToOutIndex, index);

		if(found){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[in_vertices[i]] = newindex;
		}
	}
}

unsigned short findAdj(std::vector<unsigned short> & in_indices, int i){
	int nextI = (i+1)%3 + (i/3)*3;
	for(size_t j=0; j<in_indices.size(); j++){
		if(i != j && in_indices[i] == in_indices[j]){
			if(in_indices[nextI] == in_indices[(j+2)%3 + (j/3)*3]){
				return in_indices[(j+1)%3 + (j/3)*3];
			}
		}
	}
	return -1;
}

void genTrianglesAdjacency(
	std::vector<unsigned short> & in_indices,
	std::vector<unsigned short> & out_indices
){
	for(int i=0; i<in_indices.size(); i++){
		out_indices.push_back(in_indices[i]);
		out_indices.push_back(findAdj(in_indices, i));
	}
}