#include "object.h"

Object::Object(const char* assetPath){
	// init VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// read the model file
	vector<unsigned short> indices;
	vector<Vertex> vertices;
	loadAssImp(assetPath, indices, vertices);

	// generate adjacency data
	vector<unsigned short> adj_indices;
	genTrianglesAdjacency(indices, adj_indices);
	numVert = adj_indices.size();

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, adj_indices.size() * sizeof(unsigned short), &adj_indices[0], GL_STATIC_DRAW);
}

Object::~Object(){
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

bool Object::loadAssImp(const char * path, vector<unsigned short> & indices, vector<Vertex> & vertices){
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

	// read indices
	indices.reserve(3*mesh->mNumFaces);
	for(unsigned int i=0; i<mesh->mNumFaces; i++){
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	return true;
}

void Object::genTrianglesAdjacency(vector<unsigned short> & in_indices, vector<unsigned short> & out_indices){
	// generate half edge hashtable
	unordered_map<unsigned int, unsigned short> halfEdgeHashtable;
	for(size_t i0=0; i0<in_indices.size(); i0++){
		size_t i1 = (i0+1)%3 + (i0/3)*3;
		size_t i2 = (i0+2)%3 + (i0/3)*3;
		unsigned int he = in_indices[i0]<<16 | in_indices[i1];
		halfEdgeHashtable[he] = in_indices[i2];
	}

	// use half edge hashtable to generate adjacency data
	for(size_t i0=0; i0<in_indices.size(); i0++){
		out_indices.push_back(in_indices[i0]);
		size_t i1 = (i0+1)%3 + (i0/3)*3;
		unsigned int he = in_indices[i1]<<16 | in_indices[i0];
		auto adj = halfEdgeHashtable.find(he);
		if(adj == halfEdgeHashtable.end()){
			out_indices.push_back(-1);
		}
		else{
			out_indices.push_back(adj->second);
		}
	}
}

void Object::draw(Shader* shader, mat4 VP){
	shader->use();

	// set MVP matrix
	mat4 translate = glm::translate(mat4(1.0f), vec3(0.0f, -88.0f, 0.0f));
	mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f/100.0f));
	mat4 ModelMatrix = scale * translate;
	mat4 MVP = VP * ModelMatrix;
	shader->setUniform("MVP", MVP);

	// bind VAO
	glBindVertexArray(VAO);

	// Draw the triangles
	glDrawElements(GL_TRIANGLES_ADJACENCY, numVert, GL_UNSIGNED_SHORT, (void*)0);
}
