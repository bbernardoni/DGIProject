#pragma once

#include <glad/glad.h>

#include "logging.h"

// buffer config flags
#define FB_COLOR		(1<<1)
#define FB_DEPTH_RB		(1<<2)
#define FB_DEPTH_TEX	(1<<3)

class FrameBuffer{
public:
	FrameBuffer(int width, int height, int bufFlags);
	~FrameBuffer();

	void bind(); // binds the framebuffer
	void bindColor(); // binds the color attachment to GL_TEXTURE_2D
	void bindDepth(); // binds the depth attachment to GL_TEXTURE_2D
	void clear(); // clears the framebuffer

private:
	GLuint FBO;
	GLuint colorBuf;
	GLuint depthBuf;
	int flags;

};