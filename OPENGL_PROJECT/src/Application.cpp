#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

//Define struct holding the two strings to return in ParseShader();
struct ShaderProgramSource 
{
    std::string VertexSource;
    std::string FragmentSource;
};

//Passing shader information in
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::string line;
    std::ifstream stream(filepath);
    std::stringstream ss[2]; 
    ShaderType type = ShaderType::NONE;                             //Default to no shader
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)              //'npos' indicates no matches
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));                   //Turning shader info into a string, using a null termination
    GLCall(glCompileShader(id));

    //Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char));     //'alloca' allows to allocate on the stack dynamically
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to comile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    //Input strings are just the source code
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    //Linking the shaders to the program
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    //Now vs and fs have been linked to program, the 'intermediates' can be cleared
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

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
            -0.5f, -0.5f, //0
             0.5f, -0.5f, //1
             0.5f,  0.5f, //2
            -0.5f,  0.5f  //3 all unique index points for a square
        };

        //Index buffer
        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        //Create Vertex Array Object (necessary for 'Core Profile')
        VertexArray va;
        //Create / Bind Vertex Buffer
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        //Create Index Array
        IndexBuffer ib(indicies, 6);

        //Set path relative to project directory
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");


        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);

        //Unbind everything for the VOA
        va.Unbind();
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));
            //Rebind shader
            GLCall(glUseProgram(shader));
            //Setup uniforms
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
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

        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}