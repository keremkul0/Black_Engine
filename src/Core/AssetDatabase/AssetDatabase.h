#pragma once

// Forward declarations
class Asset;
class FileSystem;
class Logger;
class Texture;
class Shader;
class Material;
class Mesh;

#include <cstdint>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <utility>
#include <vector>
#include <functional>
#include <ctime>

// Asset metadata structure
struct AssetMetadata {
    std::string uuid;
    std::string type;
    std::string name;
    std::time_t importTime;
};

// Asset database singleton
class AssetDatabase {
public:
    static AssetDatabase& GetInstance();

    // Asset loading
    bool LoadAllAssets();

    template<typename T>
    T* LoadAsset(const std::string& assetPath);

    // Asset registration
    void RegisterAsset(const std::string& assetPath);

    // Asset imports
    bool ImportAsset(const std::string& sourcePath, const std::string& assetType);

    // Asset creation
    template<typename T>
    T* CreateAsset(const std::string& assetPath);

    // Asset unloading
    void UnloadAsset(const std::string& assetPath);
    void UnloadAllAssets();

    // Asset discovery
    std::vector<std::string> FindAssetsByType(const std::string& assetType) const;

    // Asset metadata
    bool SaveAssetMetadata(const std::string& assetPath);
    static bool LoadAssetMetadata(const std::string& metaPath, AssetMetadata &metadata);

private:
    AssetDatabase();
    ~AssetDatabase();

    // Register asset loaders
    void RegisterAssetLoaders();

    // Helper functions
    static std::string GetAssetNameFromPath(const std::string& path);
    uint32_t GenerateAssetID();
    static std::string GetAssetTypeFromExtension(const std::string& extension);

    static bool CopyAsset(const std::string &sourcePath, const std::string &targetPath);

    std::unordered_map<std::string, std::string> m_AssetPathRegistry;
    std::unordered_map<std::string, Asset*> m_LoadedAssets;
    std::unordered_map<std::type_index, std::function<Asset*(const std::string&)>> m_TypeLoaders;
    uint32_t m_NextAssetID = 1;
};

// Include Asset.h after AssetDatabase declaration to solve circular dependency
#include "Asset.h"

// Asset reference template
template<typename T>
class AssetRef {
public:
    AssetRef() = default;
    explicit AssetRef(std::string  path) : m_AssetPath(std::move(path)) {}

    T* Get() const {
        return AssetDatabase::GetInstance().LoadAsset<T>(m_AssetPath);
    }

    explicit operator bool() const { return Get() != nullptr; }
    T* operator->() const { return Get(); }

private:
    std::string m_AssetPath;
};

// Template method implementations
template<typename T>
T* AssetDatabase::LoadAsset(const std::string& assetPath) {
    // Check if asset is already loaded
    if (const auto it = m_LoadedAssets.find(assetPath); it != m_LoadedAssets.end()) {
        return static_cast<T*>(it->second);
    }

    // Find the loader for this type
    const auto typeIndex = std::type_index(typeid(T));
    const auto loaderIt = m_TypeLoaders.find(typeIndex);
    if (loaderIt == m_TypeLoaders.end()) {
        return nullptr;
    }

    // Load the asset
    Asset* asset = loaderIt->second(assetPath);
    if (!asset) {
        return nullptr;
    }

    // Cache the loaded asset
    m_LoadedAssets[assetPath] = asset;
    return static_cast<T*>(asset);
}

template<typename T>
T* AssetDatabase::CreateAsset(const std::string& assetPath) {
    T* asset = new T();
    asset->SetPath(assetPath);
    asset->SetName(GetAssetNameFromPath(assetPath));
    asset->m_ID = GenerateAssetID();

    // Register the asset
    RegisterAsset(assetPath);
    m_LoadedAssets[assetPath] = asset;

    return asset;
}