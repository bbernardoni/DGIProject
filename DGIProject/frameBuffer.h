#pragma once

#include <glad/glad.h>

#include "logging.h"

#define FB_COLOR		(1<<1)
#define FB_DEPTH_RB		(1<<2)
#define FB_DEPTH_TEX	(1<<3)

class FrameBuffer{
public:
	FrameBuffer(int width, int height, int bufFlags);
	~FrameBuffer();

	void bind();
	void bindColor();
	void bindDepth();
	void clear();

private:
	GLuint FBO;
	GLuint colorBuf;
	GLuint depthBuf;
	int flags;

};