//
// Created by Kerem on 3.03.2025.
//

#ifndef SHADER_H
#define SHADER_H

#endif //SHADER_H
#pragma once

#include <string>

/**
 * Basit bir Shader sınıfı.
 * Constructor'da, vertex ve fragment kaynak kodları alarak
 * OpenGL'de bir program derleyip linkler.
 */
class Shader
{
public:
    /**
     * @param vertexSource  Vertex shader metni (string).
     * @param fragmentSource Fragment shader metni (string).
     */
    Shader(const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * Varsayılan destructor, programı siler.
     */
    ~Shader();

    /**
     * Shader programını aktif hale getirir (glUseProgram).
     */
    void use() const;

    /**
     * Program ID geri döner (gerekirse).
     */
    unsigned int getID() const { return m_ID; }

    // İleride uniform ayarlamak için yardımcı fonksiyonlar ekleyebilirsiniz:
    // void setFloat(const std::string& name, float value);
    // void setMat4(const std::string& name, const glm::mat4& mat);
    // vs.

private:
    /**
     * OpenGL'de yaratılan program nesnesinin ID'si.
     */
    unsigned int m_ID;

    /**
     * Bir shader kaynağını (vertex veya fragment) derler.
     */
    unsigned int compileShader(unsigned int type, const std::string& source);
};
