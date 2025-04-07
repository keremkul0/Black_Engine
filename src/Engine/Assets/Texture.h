#pragma once
#include "Core/AssetDatabase/AssetDatabase.h"

class Texture : public Asset {
public:
    Texture() = default;
    ~Texture() override;

    bool Load(const std::string& path);
    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    uint32_t GetChannels() const { return m_Channels; }

private:
    uint32_t m_RendererID = 0;
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_Channels = 0;
};