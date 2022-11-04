#include "Renderer.h"
#include <iostream>


void GLClearError()
{
    while (glGetError() != GL_NO_ERROR); //GL_NO_ERROR guarenteed to be zero so (!glGetError()) also works
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error]" << " (" << error << ") FUNCTION: (" << function << ") FILE: (" << file << ") LINE: (" << line << ")" << std::endl;
        return false;
    }
    return true;
}