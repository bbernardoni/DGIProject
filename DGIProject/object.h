#pragma once

#include <vector>
#include <unordered_map>
#include <stdio.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"

using std::vector;
using std::unordered_map;
using glm::vec2;
using glm::vec3;
using glm::mat4;

struct Vertex{
	vec3 pos;
};

class Object{
public:
	Object(const char* assetPath);
	~Object();

	void draw(Shader* shader, mat4 VP);

private:
	GLuint VBO;
	GLuint EBO;
	GLuint VAO;
	
	size_t numVert;

	// model init helper functions
	bool loadAssImp(const char * path, vector<unsigned short> & indices, vector<Vertex> & vertices);
	void genTrianglesAdjacency(vector<unsigned short> & in_indices, vector<unsigned short> & out_indices);
};