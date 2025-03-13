#include "Texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Make sure to download this header file

void CheckGLError(const char* operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error after " << operation << ": 0x"
                  << std::hex << error << std::dec << std::endl;
    }
}

Texture::Texture() : m_textureID(0), m_width(0), m_height(0), m_channels(0) {
}

Texture::~Texture() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
    }
}

bool Texture::LoadFromFile(const std::string& path, bool generateMipmaps) {
    // Generate texture
    if (m_textureID == 0) {
        glGenTextures(1, &m_textureID);
    }

    // Load image data using stb_image
    stbi_set_flip_vertically_on_load(true); // Flip Y-axis during loading
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "STB error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // Determine format based on channels
    GLenum internalFormat;
    GLenum dataFormat;
    if (m_channels == 1) {
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    } else if (m_channels == 3) {
        internalFormat = GL_RGB;
        dataFormat = GL_RGB;
    } else if (m_channels == 4) {
        internalFormat = GL_RGBA;
        dataFormat = GL_RGBA;
    } else {
        std::cerr << "Unsupported number of channels: " << m_channels << std::endl;
        stbi_image_free(data);
        return false;
    }

    // Bind and set texture parameters
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    CheckGLError("glTexImage2D");

    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free image data
    stbi_image_free(data);

    m_path = path;
    return true;
}

void Texture::Bind(const unsigned int unit) const {
    // Don't bind if texture ID is 0 (indicates failure)
    if (m_textureID == 0) {
        std::cerr << "Warning: Attempting to bind invalid texture (ID=0)" << std::endl;
        return;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    CheckGLError("glBindTexture");
}