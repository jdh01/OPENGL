#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak(); //mscv compiler specific
#ifdef PR_DEBUG
    #define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__)) //# makes it a string
#elif PR_RELEASE
    #define GLCall(x) x
#endif

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR); //GL_NO_ERROR guarenteed to be zero so (!glGetError()) also works
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error]" << " (" << error << ") FUNCTION: (" << function << ") FILE: (" << file << ") LINE: (" << line<< ")" << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource //struct holding the two strings to return in ParseShader();
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::string line;
    std::ifstream stream(filepath);
    std::stringstream ss[2]; //one for vertex, one for fragment
    ShaderType type = ShaderType::NONE; //default to no shader
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) //npos means invalid string pos
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
    GLCall(glShaderSource(id, 1, &src, nullptr)); //turning shader info into a string, using a null termination
    GLCall(glCompileShader(id));

    //error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char)); //alloca allows to allocate on the stack dynamically
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
    //input strings are just the source code
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    //linking the shaders tp the program
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    //now vs and fs have been linked to program, the intermediates can be cleared
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(3); //sync with monitor refresh rate

    //Glewinit requires the above context (look at documentation).
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    //cordinates of our square
    float positions[] = {
        -0.5f, -0.5f, //0
         0.5f, -0.5f, //1
         0.5f,  0.5f, //2
        -0.5f,  0.5f  //3 all unique index points for a square
    };

    //index buffer
    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); //binding is like selecting a 'buffer' layer in photoshop
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW)); //sizeof is in bytes
   
    GLCall(glEnableVertexAttribArray(0)); //enabling for the next line to work
    //now we describe the layout
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)); //(index, size, type, normalized, stride, pointer)
    //index (only 1 attrib here) so 0, size 1-4 (2 because we have x and y), data type, normalized = do they need converting type or memory location? (no), 
    //stride = offset to next vertex, pointer = offset between attributes in bytes)

    unsigned int ibo; //index buffer object
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); //binding is like selecting a 'buffer' layer in photoshop
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW)); //sizeof is in bytes

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); //relative path from project directory
    std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    

    float r = 0.0f;
    float increment = 0.05f;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
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

    //glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}