#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr() için gerekli


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    {
        std::ifstream vShaderFile(vertexPath);
        std::ifstream fShaderFile(fragmentPath);

        // Dosyaların başarılı açılıp açılmadığını kontrol et
        if (!vShaderFile.is_open()) {
            std::cerr << "ERROR: Vertex shader dosyası bulunamadı: " << vertexPath << std::endl;
            throw std::runtime_error("Vertex shader dosyası açılamadı");
        }
        if (!fShaderFile.is_open()) {
            std::cerr << "ERROR: Fragment shader dosyası bulunamadı: " << fragmentPath << std::endl;
            throw std::runtime_error("Fragment shader dosyası açılamadı");
        }

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2) Derle (vertex shader)
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // 3) Derle (fragment shader)
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 4) Shader programını linkle
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Artık ayrı shaderlar gereksiz
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    // Uniform konumunu bul
    const int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        // Opsiyonel: uniform bulunamadı diye uyarı verebilirsiniz
        // std::cerr << "Warning: uniform '" << name << "' doesn't exist!\n";
    }
    // Matrisi uniform'a yükle
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(mat));
}

// Hata kontrolü (derleme/link)
void Shader::checkCompileErrors(const unsigned int shader, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
                      << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
                      << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
