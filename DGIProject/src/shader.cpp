#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath){
	programID = glCreateProgram();

	GLuint vertID, fragID;
	if(vertexPath){
		vertID = compileShader(vertexPath, GL_VERTEX_SHADER);
		glAttachShader(programID, vertID);
	}
	if(fragmentPath){
		fragID = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
		glAttachShader(programID, fragID);
	}

	linkShader();

	if(vertexPath){
		glDetachShader(programID, vertID);
		glDeleteShader(vertID);
	}
	if(fragmentPath){
		glDetachShader(programID, fragID);
		glDeleteShader(fragID);
	}
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath){
	programID = glCreateProgram();

	GLuint vertID, geomID, fragID;
	if(vertexPath){
		vertID = compileShader(vertexPath, GL_VERTEX_SHADER);
		glAttachShader(programID, vertID);
	}
	if(geometryPath){
		geomID = compileShader(geometryPath, GL_GEOMETRY_SHADER);
		glAttachShader(programID, geomID);
	}
	if(fragmentPath){
		fragID = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
		glAttachShader(programID, fragID);
	}

	linkShader();

	if(vertexPath){
		glDetachShader(programID, vertID);
		glDeleteShader(vertID);
	}
	if(geometryPath){
		glDetachShader(programID, geomID);
		glDeleteShader(geomID);
	}
	if(fragmentPath){
		glDetachShader(programID, fragID);
		glDeleteShader(fragID);
	}
}

Shader::~Shader(){
	glDeleteProgram(programID);
}

void Shader::use(){
	glUseProgram(programID);
}

void Shader::setUniform(const char* name, GLfloat value){
	use();
	glUniform1f(glGetUniformLocation(programID, name), value);
}
void Shader::setUniform(const char* name, GLint value){
	use();
	glUniform1i(glGetUniformLocation(programID, name), value);
}
void Shader::setUniform(const char* name, GLuint value){
	use();
	glUniform1ui(glGetUniformLocation(programID, name), value);
}
void Shader::setUniform(const char* name, vec3 value){
	use();
	glUniform3f(glGetUniformLocation(programID, name), value.x, value.y, value.z);
}
void Shader::setUniform(const char* name, mat4 value){
	use();
	glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &value[0][0]);
}

GLuint Shader::compileShader(const char* filePath, GLenum shaderType){
	GLuint shaderID = glCreateShader(shaderType);

	// read file
	std::string shaderCode;
	std::ifstream file(filePath, std::ios::in);
	ASSERT(file.is_open(), ASSERT_ABORT, ASSERT_NO_EXT);

	std::stringstream sstr;
	sstr << file.rdbuf();
	shaderCode = sstr.str();
	file.close();

	// compile shader
	printf("Compiling shader: %s\n", filePath);
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// read status
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
	// link shader
	glLinkProgram(programID);

	// read status
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