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
using glm::vec4;
using glm::mat4;

struct Vertex{
	vec3 pos;
};

class Object{
public:
	Object();
	Object(const char* assetPath, mat4 importTrans = mat4(1.0f));
	~Object();

	void draw(Shader* shader, mat4 VP);
	void draw(Shader* shader);

private:
	GLuint VBO;
	GLuint EBO;
	GLuint VAO;
	
	size_t numVert;

	// model init helper functions
	bool loadModel(const char * path, mat4 importTrans, vector<unsigned short> & indices, vector<Vertex> & vertices);
};