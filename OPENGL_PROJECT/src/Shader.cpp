#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
    //Set path relative to project directory
    ShaderProgramSource source = ParseShader(filepath); //"res/shaders/Basic.shader"

    //Create Shader
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

   
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

//Passing shader information in
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
    {
        std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}