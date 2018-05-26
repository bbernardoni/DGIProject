#include "object.h"

Object::Object(const char* assetPath){
	// init VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// read the model file
	vector<unsigned short> indices;
	vector<Vertex> vertices;
	loadModel(assetPath, indices, vertices);

	// load VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// setup attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// load EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}

Object::~Object(){
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

int succ = 0;
int fail = 0;

bool Object::loadModel(const char * path, vector<unsigned short> & indices, vector<Vertex> & vertices){
	// import file
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
		aiComponent_CAMERAS);

	const aiScene* scene = importer.ReadFile(path,
		aiProcess_RemoveComponent      |
		aiProcess_Triangulate          |
		aiProcess_JoinIdenticalVertices|
		aiProcess_OptimizeMeshes       |
		aiProcess_OptimizeGraph
	);

	if(!scene){
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	// assume 1 mesh
	const aiMesh* mesh = scene->mMeshes[0];

	// read vertices
	vertices.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		Vertex vert;
		aiVector3D pos = mesh->mVertices[i];
		vert.pos = vec3(pos.x, pos.y, pos.z);

		vertices.push_back(vert);
	}

	// generate half edge hashtable
	unordered_map<unsigned int, unsigned short> halfEdgeHashtable;
	for(size_t face=0; face<mesh->mNumFaces; face++){
		for(size_t edge=0; edge<3; edge++){
			unsigned int v0 = mesh->mFaces[face].mIndices[edge];
			unsigned int v1 = mesh->mFaces[face].mIndices[(edge+1)%3];
			unsigned int v2 = mesh->mFaces[face].mIndices[(edge+2)%3];
			unsigned int he = v0<<16 | v1;
			halfEdgeHashtable[he] = v2;
		}
	}

	// use half edge hashtable to generate adjacency data
	numVert = 6*mesh->mNumFaces;
	indices.reserve(numVert);
	unsigned int undefVert = -1;
	for(unsigned int face=0; face<mesh->mNumFaces; face++){
		for(unsigned int edge=0; edge<3; edge++){
			unsigned int v0 = mesh->mFaces[face].mIndices[edge];
			unsigned int v1 = mesh->mFaces[face].mIndices[(edge+1)%3];
			indices.push_back(v0);

			unsigned int he = v1<<16 | v0;
			auto adj = halfEdgeHashtable.find(he);
			if(adj == halfEdgeHashtable.end()){
				if(undefVert == (unsigned int)-1){
					undefVert = mesh->mNumVertices;
					Vertex vert;
					vert.pos = vec3(123456.0, -123456.0, 0.0);
					vertices.push_back(vert);
				}
				indices.push_back(undefVert);
				fail++;
			}else{
				indices.push_back(adj->second);
				succ++;
			}
		}
	}

	return true;
}


void Object::draw(Shader* shader, mat4 VP){
	shader->use();

	// set MVP matrix
	//mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f/100.0f));
	//mat4 rot = mat4(1.0f);
	//mat4 translate = glm::translate(mat4(1.0f), vec3(0.0f, -0.88f, 0.0f));
	mat4 scale = glm::scale(mat4(1.0f), vec3(1/5.0f));
	mat4 rot = glm::rotate(mat4(1.0f), 3.14159f/2.0f, vec3(0.0f, 1.0f, 0.0f));
	mat4 translate = glm::translate(mat4(1.0f), vec3(0.0f, -1.0f, 1.0f));
	mat4 ModelMatrix = translate * rot * scale;
	mat4 MVP = VP * ModelMatrix;
	shader->setUniform("MVP", MVP);

	// bind VAO
	glBindVertexArray(VAO);

	// Draw the triangles
	glDrawElements(GL_TRIANGLES_ADJACENCY, numVert, GL_UNSIGNED_SHORT, (void*)0);
}
