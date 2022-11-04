#pragma once

#include <GL/glew.h>

//Error checking
#define ASSERT(x) if (!(x)) __debugbreak(); //mscv compiler specific
#ifdef PR_DEBUG
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__)) //# makes it a string
#elif PR_RELEASE
#define GLCall(x) x
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);