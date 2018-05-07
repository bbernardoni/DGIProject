#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "objloader.h"
#include "vboindexer.h"

using std::vector;
using glm::mat4;

class Object{
public:
	Object(const char* objPath, const char* texPath);
	~Object();

	void draw(Shader* shader, mat4 VP);

private:
	GLuint VBO;
	GLuint EBO;
	GLuint VAO;
	GLuint texture;

	size_t numVert;
};