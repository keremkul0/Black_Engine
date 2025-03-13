#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>

class Texture {
public:
    Texture();
    ~Texture();

    // Load texture from file
    bool LoadFromFile(const std::string& path, bool generateMipmaps = true);

    // Bind texture to specified texture unit
    void Bind(unsigned int unit = 0) const;

    // Getters
    GLuint GetID() const { return m_textureID; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannels() const { return m_channels; }
    std::string GetPath() const { return m_path; }

private:
    GLuint m_textureID = 0;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
    std::string m_path;
};

#endif // TEXTURE_H