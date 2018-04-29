#include "object.h"

Object::Object(const char* objPath, const char* texPath){
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the texture
	texture = loadDDS(texPath);

	// Read our .obj file
	vector<Vertex> vertices;
	vector<unsigned short> indices;
	vector<Vertex> indexed_vertices;
	bool res = loadOBJ(objPath, vertices);
	indexVBO(vertices, indices, indexed_vertices);
	numVert = indices.size();

	// Load it into a VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(Vertex), &indexed_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}

Object::~Object(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
}

void Object::use(Shader* shader, mat4 VP){
	shader->use();

	// set MVP matrix
	mat4 ModelMatrix = mat4(1.0);
	mat4 MVP = VP * ModelMatrix;
	shader->setUniform("MVP", MVP);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	shader->setUniform("myTextureSampler", 0);

	glBindVertexArray(VAO);

	// Draw the triangles
	glDrawElements(GL_TRIANGLES, numVert, GL_UNSIGNED_SHORT, (void*)0);
}
