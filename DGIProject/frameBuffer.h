#pragma once

#include <glad/glad.h>

#include "logging.h"

class FrameBuffer{
public:
	FrameBuffer(int width, int height, bool hasDepthBuf);
	~FrameBuffer();

	void bind();
	void bindColor();
	void clear();

private:
	GLuint FBO;
	GLuint colorBuf;
	GLuint depthBuf;

};