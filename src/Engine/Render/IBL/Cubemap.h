#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>

class Cubemap {
public:
    Cubemap();
    ~Cubemap();

    // Load cubemap from 6 individual textures
    bool LoadFromFiles(const std::vector<std::string>& facePaths);

    // Load cubemap from a single equirectangular (panoramic) HDR image
    bool LoadFromEquirectangular(const std::string& hdrPath);

    void SetID(unsigned int id);

    void Bind(unsigned int unit = 0) const;

    unsigned int GetID() const { return m_ID; }

private:
    unsigned int m_ID = 0;
    unsigned int m_Width = 0;
    unsigned int m_Height = 0;
};