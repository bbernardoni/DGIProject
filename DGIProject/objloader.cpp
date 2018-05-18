#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <string>
#include <cstring>
#include <map>

#include <glm/glm.hpp>

#include "objloader.h"

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

bool loadAssImp(const char * path, std::vector<unsigned short> & out_indices, std::vector<Vertex> & out_vertices){

	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
		aiComponent_NORMALS     | 
		aiComponent_TANGENTS_AND_BITANGENTS |
		aiComponent_COLORS      | 
		aiComponent_TEXCOORDS   |
		aiComponent_BONEWEIGHTS |
		aiComponent_ANIMATIONS  |
		aiComponent_TEXTURES    |
		aiComponent_LIGHTS      |
		aiComponent_MATERIALS   |
		aiComponent_CAMERAS     );

	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_RemoveComponent      |
		aiProcess_Triangulate          |
		aiProcess_JoinIdenticalVertices
	);

	if(!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

	// Fill vertices
	out_vertices.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		Vertex vert;
		aiVector3D pos = mesh->mVertices[i];
		vert.pos = glm::vec3(pos.x, pos.y, pos.z);
		//aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		//vert.uv = glm::vec2(UVW.x, UVW.y);
		//aiVector3D n = mesh->mNormals[i];
		//vert.norm = glm::vec3(n.x, n.y, n.z);

		out_vertices.push_back(vert);
	}

	// Fill face indices
	out_indices.reserve(3*mesh->mNumFaces);
	for(unsigned int i=0; i<mesh->mNumFaces; i++){
		// Assume the model has only triangles.
		out_indices.push_back(mesh->mFaces[i].mIndices[0]);
		out_indices.push_back(mesh->mFaces[i].mIndices[1]);
		out_indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	// The "scene" pointer will be deleted automatically by "importer"
	return true;
}

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

void genTrianglesAdjacency(
	std::vector<unsigned short> & in_indices,
	std::vector<unsigned short> & out_indices
){
	std::unordered_map<unsigned int, unsigned short> halfEdgeHashtable;
	for(size_t i0=0; i0<in_indices.size(); i0++){
		size_t i1 = (i0+1)%3 + (i0/3)*3;
		size_t i2 = (i0+2)%3 + (i0/3)*3;
		unsigned int he = in_indices[i0]<<16 | in_indices[i1];
		halfEdgeHashtable[he] = in_indices[i2];
	}
	for(size_t i0=0; i0<in_indices.size(); i0++){
		out_indices.push_back(in_indices[i0]);
		size_t i1 = (i0+1)%3 + (i0/3)*3;
		unsigned int he = in_indices[i1]<<16 | in_indices[i0];
		auto adj = halfEdgeHashtable.find(he);
		if(adj == halfEdgeHashtable.end())
			out_indices.push_back(-1);
		else
			out_indices.push_back(adj->second);
	}
}