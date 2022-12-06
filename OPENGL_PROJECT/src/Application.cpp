#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //Set Core Profile to version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Sync frame rate with monitor refresh rate
    glfwSwapInterval(3); 

    //Glewinit can now be called with context glfwMakeContextCurrent (look at documentation).
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        //One attribute holding several 'Vertex Positions'
        float positions[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, //0
             0.5f, -0.5f, 1.0f, 0.0f, //1
             0.5f,  0.5f, 1.0f, 1.0f, //2
            -0.5f,  0.5f, 0.0f, 1.0f  //3 all unique index points for a square
        };

        //Index buffer
        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //Create Vertex Array Object (necessary for 'Core Profile')
        VertexArray va;

        //Create / Bind Vertex Buffer
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        //Create Layout
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        //Create Index Array
        IndexBuffer ib(indicies, 6);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/okay-removebg-preview.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //Unbind everything for the VOA
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();
            //Rebind shader
            shader.Bind();
            //Setup uniforms
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            //Bind Vertex Buffer
            va.Bind();
            //Bind index buffer
            ib.Bind();
            //Draw call
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //glDrawElements(mode, count, type, index pointer to first)

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0)
                increment = 0.05f;

            r += increment;

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }
    }
    glfwTerminate();
    return 0;
}