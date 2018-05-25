#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>

#include <SDL.h>
#include <glad/glad.h>

enum AssertAction{
	ASSERT_LOG,
	ASSERT_RETRY,
	ASSERT_BREAK,
	ASSERT_ABORT
};

enum AssertExtra{
	ASSERT_NO_EXT,
	ASSERT_SDL,
	ASSERT_GL
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define ASSERT_STR(COND) __FUNCTION__ "() "__FILE__ ":" TOSTRING(__LINE__) "\n    " #COND

#define ASSERT(COND,ACT,EXT) \
	do{ \
		while(!(COND)){ \
			if(EXT == ASSERT_NO_EXT) puts(ASSERT_STR(COND)); \
			if(EXT == ASSERT_SDL) printf(ASSERT_STR(COND)", SDL Error: %s\n", SDL_GetError()); \
			if(EXT == ASSERT_GL) printf(ASSERT_STR(COND)", OpenGL Error: 0x%04x\n", glGetError()); \
			\
			if(ACT == ASSERT_ABORT){ system("pause"); exit(-1); } \
			else if(ACT == ASSERT_BREAK) __debugbreak(); \
			if(ACT != ASSERT_RETRY) break; \
		} \
	} while(0)
