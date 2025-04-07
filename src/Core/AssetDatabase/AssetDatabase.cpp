/**
* @file AssetDatabase.cpp
 * @brief Implementation of an asset management system for Black Engine
 *
 * The AssetDatabase manages loading, unloading and tracking of all engine assets,
 * providing a centralized system for asset references and metadata.
 */

#include "AssetDatabase.h"
#include "Core/FileSystem/FileSystem.h"
#include "Engine/Render/Mesh/Mesh.h"
#include "Engine/Render/Shader/Shader.h"
#include <nlohmann/json.hpp>

#include "Core/Logger/LogMacros.h"
#include "Core/ProjectManager/ProjectManager.h"

using json = nlohmann::json;

// Get singleton instance
AssetDatabase &AssetDatabase::GetInstance() {
    static AssetDatabase instance;
    return instance;
}

// Constructor
AssetDatabase::AssetDatabase() {
    // Initialize logger when AssetDatabase is actually used
    BE_CAT_INFO("Asset", "Asset database initializing");

    RegisterAssetLoaders();

    BE_CAT_DEBUG("Asset", "Asset database initialized");
}

// Destructor
AssetDatabase::~AssetDatabase() {
    BE_CAT_INFO("Asset", "Asset database shutting down");
    UnloadAllAssets();
}

/**
 * @brief Register loaders for different asset types
 */
void AssetDatabase::RegisterAssetLoaders() {
    // Register mesh loader
    m_TypeLoaders[std::type_index(typeid(Mesh))] = [](const std::string &path) -> Asset *{
        BE_CAT_DEBUG_FORMAT("Asset", "Loading mesh from: {}", path);

        auto *mesh = new Mesh();
        mesh->SetPath(path);
        mesh->SetName(FileSystem::GetFileNameWithoutExtension(path));
        mesh->SetLoaded(true);

        return static_cast<Asset *>(mesh);
    };

    // Register shader loader
    m_TypeLoaders[std::type_index(typeid(Shader))] = [](const std::string &path) -> Asset *{
        BE_CAT_DEBUG_FORMAT("Asset", "Loading shader from: {}", path);

        auto *shader = new Shader();
        shader->SetPath(path);
        shader->SetName(FileSystem::GetFileNameWithoutExtension(path));
        shader->SetLoaded(true);

        return static_cast<Asset *>(shader);
    };

    BE_CAT_INFO_FORMAT("Asset", "Registered {} asset type loaders", m_TypeLoaders.size());
}


/**
 * @brief Load all assets in the project directory
 * @return True if assets were loaded successfully
 */
bool AssetDatabase::LoadAllAssets() {
    BE_CAT_INFO("Asset", "Beginning asset discovery process");

    // Get a project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    const std::string assetsPath = projectPath + "/Assets";

    BE_CAT_DEBUG_FORMAT("Asset", "Scanning assets directory: {}", assetsPath);

    // Check if assets directory exists
    if (!FileSystem::DirectoryExists(assetsPath)) {
        if (!FileSystem::CreateDirectory(assetsPath)) {
            BE_CAT_ERROR("Asset", "Failed to create assets directory");
            return false;
        }
    }

    // Create asset type directories if they don't exist
    for (std::vector<std::string> assetTypes = {"Meshes", "Textures", "Shaders", "Materials", "Scenes"}; const auto &
         type: assetTypes) {
        if (std::string typePath = assetsPath + "/" + type; !FileSystem::DirectoryExists(typePath)) {
            FileSystem::CreateDirectory(typePath);
        }
    }

    // Define the scanning function
    std::function<void(const std::string &)> scanDirectory;

    scanDirectory = [this, &scanDirectory](const std::string &dirPath) {
        for (auto files = FileSystem::GetFilesInDirectory(dirPath); const auto &file: files) {
            if (std::string ext = FileSystem::GetFileExtension(file); ext == ".meta") continue; // Skip metadata files

            RegisterAsset(file);
        }

        for (auto subdirs = FileSystem::GetDirectoriesInDirectory(dirPath); const auto &subdir: subdirs) {
            scanDirectory(subdir);
        }
    };

    // Start scanning from the assets directory
    scanDirectory(assetsPath);

    BE_CAT_INFO("Asset", "Asset discovery complete");
    return true;
}

// Register an asset in the database
void AssetDatabase::RegisterAsset(const std::string &assetPath) {
    const std::string assetName = FileSystem::GetFileNameWithoutExtension(assetPath);
    m_AssetPathRegistry[assetName] = assetPath;
}

// Import an asset from external source
bool AssetDatabase::ImportAsset(const std::string &sourcePath, const std::string &assetType) {
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    const std::string assetsPath = projectPath + "/Assets";
    const std::string fileName = FileSystem::GetFileName(sourcePath);
    std::string targetPath;

    // Determine a target path based on an asset type
    if (assetType == "Mesh") {
        targetPath = assetsPath + "/Meshes/" + fileName;
    } else if (assetType == "Texture") {
        targetPath = assetsPath + "/Textures/" + fileName;
    } else if (assetType == "Shader") {
        targetPath = assetsPath + "/Shaders/" + fileName;
    } else if (assetType == "Material") {
        targetPath = assetsPath + "/Materials/" + fileName;
    } else {
        return false;
    }

    // Check if a source file exists
    if (!FileSystem::FileExists(sourcePath)) {
        BE_CAT_ERROR_FORMAT("Asset", "Source file not found: {}", sourcePath);
        return false;
    }

    // Read a source file
    const std::vector<uint8_t> data = FileSystem::ReadBinaryFile(sourcePath);
    if (data.empty()) {
        BE_CAT_ERROR_FORMAT("Asset", "Failed to read source file: {}", sourcePath);
        return false;
    }

    // Write to a target file
    if (!FileSystem::WriteBinaryFile(targetPath, data)) {
        BE_CAT_ERROR_FORMAT("Asset", "Failed to write target file: {}", targetPath);
        return false;
    }

    // Register the new asset
    RegisterAsset(targetPath);

    // Create metadata
    SaveAssetMetadata(targetPath);

    BE_CAT_INFO("Asset", "Successfully loaded asset database");
    return true;
}

// Unload a specific asset from memory
void AssetDatabase::UnloadAsset(const std::string &assetPath) {
    if (const auto it = m_LoadedAssets.find(assetPath); it != m_LoadedAssets.end()) {
        delete it->second;
        BE_CAT_INFO_FORMAT("Asset", "Unloaded asset: {}", assetPath);
        m_LoadedAssets.erase(it);
    }
}

// Unload all assets from memory
void AssetDatabase::UnloadAllAssets() {
    for (const auto &asset: m_LoadedAssets | std::views::values) {
        delete asset;
    }
    BE_CAT_INFO_FORMAT("Asset", "Unloaded {} assets", m_LoadedAssets.size());
    m_LoadedAssets.clear();
}

// Find assets of a specific type
std::vector<std::string> AssetDatabase::FindAssetsByType(const std::string &assetType) const {
    std::vector<std::string> result;

    // Determine file extension for a given asset type
    std::string extension;
    if (assetType == "Mesh") extension = ".obj";
    else if (assetType == "Texture") extension = ".png";
    else if (assetType == "Shader") extension = ".glsl";

    // Find all assets with matching extension
    for (auto it = m_AssetPathRegistry.begin(); it != m_AssetPathRegistry.end(); ++it) {
        if (FileSystem::GetFileExtension(it->second) == extension) {
            result.push_back(it->second);
        }
    }

    return result;
}

// Save metadata for an asset
bool AssetDatabase::SaveAssetMetadata(const std::string &assetPath) {
    // Create metadata JSON
    json metadata;
    metadata["assetId"] = GenerateAssetID();
    metadata["type"] = GetAssetTypeFromExtension(FileSystem::GetFileExtension(assetPath));
    metadata["importTime"] = std::time(nullptr);

    // Write a metadata file
    const std::string metaPath = assetPath + ".meta";
    return FileSystem::WriteTextFile(metaPath, metadata.dump(4));
}

// Load metadata for an asset
bool AssetDatabase::LoadAssetMetadata(const std::string &metaPath, AssetMetadata &metadata) {
    if (!FileSystem::FileExists(metaPath)) {
        BE_CAT_ERROR_FORMAT("Asset", "Metadata file not found: {}", metaPath);
        return false;
    }

    try {
        std::string content = FileSystem::ReadTextFile(metaPath);
        json j = json::parse(content);

        metadata.uuid = j["uuid"];
        metadata.type = j["type"];
        metadata.name = j["name"];

        return true;
    } catch (const std::exception &e) {
        // Log error
        BE_CAT_ERROR_FORMAT("Asset", "Failed to load metadata from '{}': {}", metaPath, e.what());
        return false;
    }
}

// Get asset name from a path
std::string AssetDatabase::GetAssetNameFromPath(const std::string &path) {
    return FileSystem::GetFileNameWithoutExtension(path);
}

// Generate unique asset ID
uint32_t AssetDatabase::GenerateAssetID() {
    return m_NextAssetID++;
}

// Get an asset type from file extension
std::string AssetDatabase::GetAssetTypeFromExtension(const std::string &extension) {
    if (extension == ".obj" || extension == ".fbx" || extension == ".gltf") return "Mesh";
    if (extension == ".png" || extension == ".jpg" || extension == ".tga") return "Texture";
    if (extension == ".glsl" || extension == ".vert" || extension == ".frag") return "Shader";
    if (extension == ".mat") return "Material";
    return "Unknown";
}

bool AssetDatabase::CopyAsset(const std::string &sourcePath, const std::string &targetPath) {
    const std::vector<uint8_t> data = FileSystem::ReadBinaryFile(sourcePath);
    if (data.empty()) {
        BE_CAT_ERROR_FORMAT("Asset", "Failed to read source file: {}", sourcePath);
        return false;
    }

    // Write to a target file
    if (!FileSystem::WriteBinaryFile(targetPath, data)) {
        BE_CAT_ERROR_FORMAT("Asset", "Failed to write target file: {}", targetPath);
        return false;
    }
    return true;
}