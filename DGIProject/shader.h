#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::mat4;

class Shader{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
	~Shader();

	void use();

	void setUniform(const char* name, GLfloat value);
	void setUniform(const char* name, GLint value);
	void setUniform(const char* name, GLuint value);
	void setUniform(const char* name, vec3 value);
	void setUniform(const char* name, mat4 value);

private:
	GLuint programID;

	GLuint compileShader(const char* filePath, GLenum shaderType);
	void linkShader();
};