#include "Shader.h"
#include <glad/glad.h>
#include <iostream>

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
    // 1) Vertex ve fragment shader'ları derle
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // 2) Program oluştur ve shader'ları linkle
    m_ID = glCreateProgram();
    glAttachShader(m_ID, vs);
    glAttachShader(m_ID, fs);
    glLinkProgram(m_ID);

    // 3) Link hata kontrolü
    int success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
        std::cerr << "Shader Program link error:\n" << infoLog << std::endl;
    }

    // 4) Artık ayrı shader objelerini silebiliriz
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(m_ID);
}

void Shader::use() const
{
    glUseProgram(m_ID);
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
    // 1) Shader oluştur
    unsigned int shader = glCreateShader(type);

    // 2) Kaynağı derleyiciye ver
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // 3) Hata kontrolü
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error:\n" << infoLog << std::endl;
    }

    return shader;
}
