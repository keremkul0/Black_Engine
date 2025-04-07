#ifndef SHADER_H
#define SHADER_H

#include "Core/AssetDatabase/Asset.h"  // Include Asset base class
#include <string>
#include <glm/glm.hpp>

class Shader : public Asset  // Inherit from Asset
{
public:
    // OpenGL shader program id
    unsigned int ID{};

    Shader() = default;
    // Constructor: takes vertex and fragment shader paths
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader() override = default;  // Mark as override

    // Shader methods
    void use() const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    // You can add more uniform setters as needed:
    // void setVec3(const std::string &name, const glm::vec3 &value) const;
    // void setFloat(const std::string &name, float value) const;
    // void setInt(const std::string &name, int value) const;

private:
    // Compilation/link functions
    static void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif