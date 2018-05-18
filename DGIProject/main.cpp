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

Shader* shader = NULL;
Shader* shaderBlur = NULL;
Object* monkey = NULL;
FrameBuffer* raw = NULL;
FrameBuffer* ping = NULL;
FrameBuffer* pong = NULL;

//Graphics variables
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
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		LOG_EXIT_SDL(Error, Failed to initialize SDL subsystems);

	atexit(close);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	gWindow = SDL_CreateWindow("OpenGL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

	// Create and compile our GLSL program from the shaders
	shader = new Shader("StandardShading.vert", "StandardShading.geom", "StandardShading.frag");
	shaderBlur = new Shader("blur.vert", "blur.frag");

	monkey = new Object("dummy_obj.obj", "uvmap.DDS");

	raw = new FrameBuffer(SCR_WIDTH, SCR_HEIGHT, true);
	ping = new FrameBuffer(SCR_WIDTH, SCR_HEIGHT, false);
	pong = new FrameBuffer(SCR_WIDTH, SCR_HEIGHT, false);
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
	//ProjectionMatrix = glm::ortho(-2.0f, 2.0f, -3.0f/2.0f, 3.0f/2.0f, 0.1f, 100.0f);
	ViewMatrix = glm::translate(glm::inverse(camRot), -position);

	if(keyState[SDL_SCANCODE_ESCAPE])
		exit(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if(quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void render(){
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 VP = ProjectionMatrix * ViewMatrix;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// draw monkey
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//raw->bind();
	//raw->clear();
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if(keyState[SDL_SCANCODE_Z])
		VP[1][3] += 0;
	monkey->draw(shader, VP);

	// blur bright fragments with two-pass Gaussian Blur 
	/*bool horizontal = true;
	unsigned int amount = 2;
	shaderBlur->use();
	shaderBlur->setUniform("image", 0);
	FrameBuffer* inputBuf = raw;
	FrameBuffer* outputBuf = pong;
	for(unsigned int i = 0; i < amount; i++){
		if(i == amount-1){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}else{
			outputBuf->bind();
		}
		shaderBlur->setUniform("horizontal", horizontal);
		inputBuf->bindColor();
		renderQuad();
		horizontal = !horizontal;
		inputBuf = horizontal? ping: pong;
		outputBuf = horizontal? pong: ping;
	}*/
}

void close(){
	delete monkey;
	delete shader;
	delete shaderBlur;

	delete raw;
	delete ping;
	delete pong;

	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}
