#include "frameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, int bufFlags){
	// init FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// add attachments
	flags = bufFlags;
	if(flags & FB_COLOR){ // add color texture
		glGenTextures(1, &colorBuf);
		glBindTexture(GL_TEXTURE_2D, colorBuf);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuf, 0);
	}

	if(flags & FB_DEPTH_RB){ // add depth renderbuffer
		glGenRenderbuffers(1, &depthBuf);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	}else if(flags & FB_DEPTH_TEX){ // add depth texture
		glGenTextures(1, &depthBuf);
		glBindTexture(GL_TEXTURE_RECTANGLE, depthBuf);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, depthBuf, 0);
	}

	// check if framebuffer is complete
	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, ASSERT_ABORT, ASSERT_GL);
}

FrameBuffer::~FrameBuffer(){
	// delete everything
	glDeleteFramebuffers(1, &FBO);
	if(flags & FB_COLOR)
		glDeleteTextures(1, &colorBuf);
	if(flags & FB_DEPTH_RB)
		glDeleteRenderbuffers(1, &depthBuf);
	else if(flags & FB_DEPTH_TEX)
		glDeleteTextures(1, &depthBuf);
}

void FrameBuffer::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::bindColor(){
	glBindTexture(GL_TEXTURE_2D, colorBuf);
}

void FrameBuffer::bindDepth(){
	glBindTexture(GL_TEXTURE_RECTANGLE, depthBuf);
}

void FrameBuffer::clear(){
	if(flags & FB_COLOR)
		glClear(GL_COLOR_BUFFER_BIT);
	if(flags & (FB_DEPTH_RB|FB_DEPTH_TEX))
		glClear(GL_DEPTH_BUFFER_BIT);
}