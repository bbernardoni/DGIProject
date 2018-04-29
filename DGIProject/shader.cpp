#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath){
	programID = glCreateProgram();

	GLuint vertID = compileShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragID = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
	glAttachShader(programID, vertID);
	glAttachShader(programID, fragID);

	linkShader();

	glDetachShader(programID, vertID);
	glDetachShader(programID, fragID);
	glDeleteShader(vertID);
	glDeleteShader(fragID);
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath){
	programID = glCreateProgram();

	GLuint vertID = compileShader(vertexPath, GL_VERTEX_SHADER);
	GLuint geomID = compileShader(geometryPath, GL_GEOMETRY_SHADER);
	GLuint fragID = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
	glAttachShader(programID, vertID);
	glAttachShader(programID, geomID);
	glAttachShader(programID, fragID);

	linkShader();

	glDetachShader(programID, vertID);
	glDetachShader(programID, geomID);
	glDetachShader(programID, fragID);
	glDeleteShader(vertID);
	glDeleteShader(geomID);
	glDeleteShader(fragID);
}

Shader::~Shader(){
	glDeleteProgram(programID);
}

void Shader::use(){
	glUseProgram(programID);
}

void Shader::setUniform(const char* name, GLfloat value){
	glUniform1f(glGetUniformLocation(programID, name), value);
}
void Shader::setUniform(const char* name, GLint value){
	glUniform1i(glGetUniformLocation(programID, name), value);
}
void Shader::setUniform(const char* name, GLuint value){
	glUniform1ui(glGetUniformLocation(programID, name), value);
}
void Shader::setUniform(const char* name, vec3 value){
	glUniform3f(glGetUniformLocation(programID, name), value.x, value.y, value.z);
}
void Shader::setUniform(const char* name, mat4 value){
	glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &value[0][0]);
}

GLuint Shader::compileShader(const char* filePath, GLenum shaderType){
	GLuint shaderID = glCreateShader(shaderType);

	std::string shaderCode;
	std::ifstream file(filePath, std::ios::in);
	if(file.is_open()){
		std::stringstream sstr;
		sstr << file.rdbuf();
		shaderCode = sstr.str();
		file.close();
	}
	else{
		// error
	}

	printf("Compiling shader: %s\n", filePath);
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	GLint status = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if(!status){
		int logLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> msg(logLen+1);
		glGetShaderInfoLog(shaderID, logLen, NULL, &msg[0]);
		printf("%s\n", &msg[0]);
	}

	return shaderID;
}

void Shader::linkShader(){
	glLinkProgram(programID);

	GLint status = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if(!status){
		int logLen;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> msg(logLen+1);
		glGetProgramInfoLog(programID, logLen, NULL, &msg[0]);
		printf("%s\n", &msg[0]);
	}
}