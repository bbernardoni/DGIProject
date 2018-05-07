#include "frameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, bool hasDepthBuf){
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorBuf);
	glBindTexture(GL_TEXTURE_2D, colorBuf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuf, 0);

	// create and attach depth buffer (renderbuffer)
	if(hasDepthBuf){
		glGenRenderbuffers(1, &depthBuf);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	}else{
		depthBuf = 0;
	}

	// finally check if framebuffer is complete
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_EXIT(Error, Framebuffer not complete);
}

FrameBuffer::~FrameBuffer(){
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &colorBuf);
	if(depthBuf)
		glDeleteRenderbuffers(1, &depthBuf);
}

void FrameBuffer::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::bindColor(){
	glBindTexture(GL_TEXTURE_2D, colorBuf);
}

void FrameBuffer::clear(){
	if(depthBuf)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);
}