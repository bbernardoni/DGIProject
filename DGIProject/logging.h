#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define LOG_STR(TYP,CMSG) #TYP ": " __FILE__ " " __FUNCTION__ "() " TOSTRING(__LINE__) " - " #CMSG

#define LOG(TYP,CMSG)		do{ puts(LOG_STR(TYP,CMSG)); } while(0)
#define LOG_SDL(TYP,CMSG)	do{ printf(LOG_STR(TYP,CMSG)", SDL Error: %s\n", SDL_GetError()); } while(0)

#define LOG_EXIT(TYP,CMSG)		do{ LOG(TYP,CMSG); exit(-1); } while(0)
#define LOG_EXIT_SDL(TYP,CMSG)	do{ LOG_SDL(TYP,CMSG); exit(-1); } while(0)