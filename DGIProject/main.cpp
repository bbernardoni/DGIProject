#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_opengl.h>

#include "shader.h"
#include "object.h"
#include "frameBuffer.h"
#include "logging.h"

using glm::vec2;
using glm::vec3;
using glm::mat4;
using std::vector;

// constants
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

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

// graphics objects
Shader* shader = NULL;
Shader* shaderDepth = NULL;
Object* hallway = NULL;
Object* dummy = NULL;
FrameBuffer* depthBuf = NULL;

// graphics matrices
mat4 ViewMatrix;
mat4 ProjectionMatrix;

Uint32 deltaTime, frame;

// camera vars
vec3 position = vec3(0, 0, 5);
float yaw = 0.0f;
float pitch = 0.0f;

int main(int argc, char* args[]){
	init();

	SDL_Event e;
	Uint32 lastTime = SDL_GetTicks();
	frame = 0;
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
		frame++;
	}
	return 0;
}

void init(){
	// init SDL
	ASSERT(SDL_Init(SDL_INIT_VIDEO) >= 0, ASSERT_ABORT, ASSERT_SDL);

	atexit(close);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	gWindow = SDL_CreateWindow("Silhouette Rendering with Vector Monitor Lines", 
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	ASSERT(gWindow != NULL, ASSERT_ABORT, ASSERT_SDL);

	gContext = SDL_GL_CreateContext(gWindow);
	ASSERT(gContext != NULL, ASSERT_ABORT, ASSERT_SDL);

	ASSERT(SDL_GL_SetSwapInterval(0) >= 0, ASSERT_LOG, ASSERT_SDL);

	// init GLAD
	ASSERT(gladLoadGLLoader(SDL_GL_GetProcAddress), ASSERT_LOG, ASSERT_NO_EXT);

	// init OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_MAX);

	// create shaders
	shader = new Shader("shaders/transformLines.vert", "shaders/calcLines.geom", "shaders/vectorLines.frag");
	shader->setUniform("DepthSampler", 0);
	shaderDepth = new Shader("shaders/transform.vert", NULL);

	// create models
	mat4 scale = glm::scale(mat4(1.0f), vec3(1/5.0f));
	mat4 rot = glm::rotate(mat4(1.0f), 3.14159f/2.0f, vec3(0.0f, 1.0f, 0.0f));
	mat4 translate = glm::translate(mat4(1.0f), vec3(0.0f, -1.0f, 1.0f));
	hallway = new Object("Ship Hallway.lwo", translate*rot*scale);

	scale = glm::scale(mat4(1.0f), vec3(1.0f/100.0f));
	translate = glm::translate(mat4(1.0f), vec3(0.0f, -0.98f, 0.0f));
	dummy = new Object("dummy_obj.obj", translate*scale);

	// create depth framebuffer
	depthBuf = new FrameBuffer(SCR_WIDTH, SCR_HEIGHT, FB_DEPTH_TEX);
}

void update(){
	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(gWindow, SCR_WIDTH/2, SCR_HEIGHT/2);

	// update camera angles
	if(frame > 1){
		yaw   += float(SCR_WIDTH/2 - x)  * (float)deltaTime * mouseSpeed;
		pitch += float(SCR_HEIGHT/2 - y) * (float)deltaTime * mouseSpeed;
	}

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

	// quit on escape
	if(keyState[SDL_SCANCODE_ESCAPE])
		exit(0);
}

void render(){
	mat4 VP = ProjectionMatrix * ViewMatrix;

	// draw depth buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	depthBuf->bind();
	depthBuf->clear();
	dummy->draw(shaderDepth, VP);
	hallway->draw(shaderDepth, VP);

	// draw stylized lines
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	depthBuf->bindDepth();
	dummy->draw(shader, VP);
	hallway->draw(shader, VP);
}

void close(){
	// deallocate objects
	delete hallway;
	delete dummy;
	delete shader;
	delete shaderDepth;
	delete depthBuf;

	// shutdown SDL
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}
