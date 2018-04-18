#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_opengl.h>

#include "shader.h"
#include "texture.h"
#include "objloader.h"
#include "vboindexer.h"

using glm::vec2;
using glm::vec3;
using glm::mat4;
using std::vector;

// constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const float fov = glm::radians(45.0f);
const float transSpeed = 0.003f;
const float mouseSpeed = 0.0001f;

// function prototypes
bool init();
bool initGL();

void update();
void render();

void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Graphics variables
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;
GLuint VertexArrayID;
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint programID;
GLuint Texture;
GLuint TextureID;
GLuint LightID;

vector<vec3> vertices;
vector<vec2> uvs;
vector<vec3> normals;

vector<unsigned short> indices;
vector<vec3> indexed_vertices;
vector<vec2> indexed_uvs;
vector<vec3> indexed_normals;

mat4 ViewMatrix;
mat4 ProjectionMatrix;

Uint32 deltaTime;

// camera vars
vec3 position = vec3(0, 0, 5);
float yaw = 0.0f;
float pitch = 0.0f;

bool init(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	if(gWindow == NULL){
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	
	gContext = SDL_GL_CreateContext( gWindow );
	if(gContext == NULL){
		printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
	}

	if(SDL_GL_SetSwapInterval( 1 ) < 0){
		printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
	}

	if(!initGL()){
		printf( "Unable to initialize OpenGL!\n" );
		return false;
	}

	SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

	return true;
}

bool initGL(){
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vert", "StandardShading.geom", "StandardShading.frag");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	Texture = loadDDS("uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	
	// Read our .obj file
	bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	
	return true;
}

void update(){
	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

	// update camera angles
	yaw   += float(SCREEN_WIDTH/2 - x)  * (float)deltaTime * mouseSpeed;
	pitch += float(SCREEN_HEIGHT/2 - y) * (float)deltaTime * mouseSpeed;

	// calculate rotation matrix
	mat4 camRot = glm::rotate(mat4(), yaw, vec3(0, 1, 0));
	camRot = glm::rotate(camRot, pitch, vec3(1, 0, 0));

	// update camera position
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if(keyState[SDL_SCANCODE_D])
		position += vec3(camRot[0] * (float)deltaTime * transSpeed);
	if(keyState[SDL_SCANCODE_A])
		position -= vec3(camRot[0] * (float)deltaTime * transSpeed);
	if(keyState[SDL_SCANCODE_Q])
		position += vec3(camRot[1] * (float)deltaTime * transSpeed);
	if(keyState[SDL_SCANCODE_E])
		position -= vec3(camRot[1] * (float)deltaTime * transSpeed);
	if(keyState[SDL_SCANCODE_S])
		position += vec3(camRot[2] * (float)deltaTime * transSpeed);
	if(keyState[SDL_SCANCODE_W])
		position -= vec3(camRot[2] * (float)deltaTime * transSpeed);

	// calculate project and view matrix
	ProjectionMatrix = glm::perspective(fov, 4.0f / 3.0f, 0.1f, 100.0f);
	ViewMatrix = glm::translate(glm::inverse(camRot), -position);
}

void render(){
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	// Compute the MVP matrix from keyboard and mouse input
	mat4 ModelMatrix = mat4(1.0);
	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	vec3 lightPos = vec3(4, 4, 4);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void close(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[]){
	//Start up SDL and create window
	if(!init()){
		printf( "Failed to initialize!\n" );
		return -1;
	}

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;
		
	//Enable text input
	SDL_StartTextInput();

	Uint32 lastTime = SDL_GetTicks();

	//While application is running
	while( !quit )
	{
		// Compute time difference between current and last frame
		Uint32 currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastTime;

		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}

		update();

		//Render quad
		render();
			
		//Update screen
		SDL_GL_SwapWindow( gWindow );

		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;
	}
		
	//Disable text input
	SDL_StopTextInput();

	//Free resources and close SDL
	close();
	return 0;
}
