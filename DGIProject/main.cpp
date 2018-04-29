#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_opengl.h>

#include "shader.h"
#include "logging.h"
#include "texture.h"
#include "objloader.h"
#include "vboindexer.h"

using glm::vec2;
using glm::vec3;
using glm::mat4;
using std::vector;

// constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const float fov = glm::radians(45.0f);
const float transSpeed = 0.003f;
const float mouseSpeed = 0.0001f;

// function prototypes
void init();
void update();
void render();
void close();

// SDL globals
SDL_Window* gWindow = NULL;
SDL_GLContext gContext;

Shader* shader = NULL;

//Graphics variables
GLuint vertexbuffer;
GLuint elementbuffer;
GLuint VertexArrayID;
GLuint Texture;

vector<unsigned short> indices;
vector<Vertex> indexed_vertices;

mat4 ViewMatrix;
mat4 ProjectionMatrix;

Uint32 deltaTime;

// camera vars
vec3 position = vec3(0, 0, 5);
float yaw = 0.0f;
float pitch = 0.0f;

int main(int argc, char* args[]){
	init();

	SDL_Event e;
	Uint32 lastTime = SDL_GetTicks();
	while(true){
		Uint32 currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastTime;

		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				return 0;
			}
		}

		update();

		render();
		SDL_GL_SwapWindow(gWindow);

		lastTime = currentTime;
	}
	return 0;
}

void init(){
	// init SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		LOG_EXIT_SDL(Error, Failed to initialize SDL subsystems);

	atexit(close);

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	gWindow = SDL_CreateWindow("OpenGL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(gWindow == NULL)
		LOG_EXIT_SDL(Error, Failed to create window);

	gContext = SDL_GL_CreateContext(gWindow);
	if(gContext == NULL)
		LOG_EXIT_SDL(Error, Failed to create OpenGL context);

	if(SDL_GL_SetSwapInterval(1) < 0)
		LOG_SDL(Warning, Unable to set VSync);

	// init GLAD
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress))
		LOG_EXIT(Error, Failed to initialize GLAD);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	shader = new Shader("StandardShading.vert", "StandardShading.geom", "StandardShading.frag");

	// Load the texture
	Texture = loadDDS("uvmap.DDS");

	// Read our .obj file
	vector<Vertex> vertices;
	bool res = loadOBJ("suzanne.obj", vertices);
	indexVBO(vertices, indices, indexed_vertices);

	// Load it into a VBO
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(Vertex), &indexed_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);


	SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
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

	if(keyState[SDL_SCANCODE_ESCAPE])
		exit(0);
}

void render(){
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	shader->use();

	// Compute the MVP matrix from keyboard and mouse input
	mat4 ModelMatrix = mat4(1.0);
	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	shader->setUniform("MVP", MVP);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	shader->setUniform("myTextureSampler", 0);

	glBindVertexArray(VertexArrayID);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
}

void close(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	delete shader;

	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}
