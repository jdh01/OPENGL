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

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    //Bind shader
    shader.Bind();
    //Bind Vertex Buffer
    va.Bind();
    //Bind index buffer
    ib.Bind();
    //Draw call
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr)); //glDrawElements(mode, count, type, index pointer to first)

}
