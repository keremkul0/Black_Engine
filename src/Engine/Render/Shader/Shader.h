#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    // OpenGL shader program id
    unsigned int ID;

    // Kurucu: Vertex ve fragment shader dosya yollarını alıp shader programını oluşturur
    Shader(const char* vertexPath, const char* fragmentPath);

    // Shader'ı aktif et
    void use() const;

    // setMat4 fonksiyonunu tanımlarız:
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    // İhtiyaca göre setVec3, setFloat, setInt vb. de ekleyebilirsiniz:
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    // void setFloat(const std::string &name, float value) const;
    // void setInt(const std::string &name, int value) const;

private:
    // Derleme/link fonksiyonları vs.
    static void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif

