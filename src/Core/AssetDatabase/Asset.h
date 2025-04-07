// src/Core/AssetDatabase/Asset.h
#pragma once

#include <string>
#include <cstdint>

// Forward declare AssetDatabase to avoid circular dependency
class AssetDatabase;

class Asset {
public:
    virtual ~Asset() = default;

    [[nodiscard]] const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    [[nodiscard]] const std::string& GetPath() const { return m_Path; }
    void SetPath(const std::string& path) { m_Path = path; }

    [[nodiscard]] uint32_t GetID() const { return m_ID; }

    void SetLoaded(bool loaded) { m_IsLoaded = loaded; }
    [[nodiscard]] bool IsLoaded() const { return m_IsLoaded; }

protected:
    std::string m_Name;
    std::string m_Path;
    uint32_t m_ID = 0;
    bool m_IsLoaded = false;

    friend class AssetDatabase;
};