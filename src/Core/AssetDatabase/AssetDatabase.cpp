#include "AssetDatabase.h"
#include "Core/FileSystem/FileSystem.h"
#include "Engine/Render/Mesh/Mesh.h"
#include "Engine/Render/Shader/Shader.h"
#include <nlohmann/json.hpp>

#include "Core/Logger/LogMacros.h"
#include "Core/ProjectManager/ProjectManager.h"

using json = nlohmann::json;

// Define and register the Asset log category
BE_DEFINE_LOG_CATEGORY(AssetLog, "Asset");

// Get singleton instance
AssetDatabase &AssetDatabase::GetInstance() {
    static AssetDatabase instance;
    return instance;
}

// Constructor
AssetDatabase::AssetDatabase() {
    BE_LOG_INFO(AssetLog, "Asset database initializing");
    RegisterAssetLoaders();
    BE_LOG_DEBUG(AssetLog, "Asset database initialized");
}

// Destructor
AssetDatabase::~AssetDatabase() {
    BE_LOG_INFO(AssetLog, "Asset database shutting down");
    UnloadAllAssets();
}

/**
 * @brief Register loaders for different asset types
 */
void AssetDatabase::RegisterAssetLoaders() {
    // Register mesh loader
    m_TypeLoaders[std::type_index(typeid(Mesh))] = [](const std::string &path) -> Asset *{
        BE_LOG_DEBUG(AssetLog, "Loading mesh from: {}", path);

        auto *mesh = new Mesh();
        mesh->SetPath(path);
        mesh->SetName(FileSystem::BE_Get_File_Name_Without_Extension(path));
        mesh->SetLoaded(true);

        return mesh;
    };

    // Register shader loader
    m_TypeLoaders[std::type_index(typeid(Shader))] = [](const std::string &path) -> Asset *{
        BE_LOG_DEBUG(AssetLog, "Loading shader from: {}", path);
        auto *shader = new Shader();
        shader->SetPath(path);
        shader->SetName(FileSystem::BE_Get_File_Name_Without_Extension(path));
        shader->SetLoaded(true);

        return shader;
    };

    BE_LOG_INFO(AssetLog, "Registered asset loaders: {}", m_TypeLoaders.size());
}


/**
 * @brief Load all assets in the project directory
 * @return True if assets were loaded successfully
 */
bool AssetDatabase::LoadAllAssets() {
    BE_LOG_INFO(AssetLog, "Beginning asset discovery process");

    // Get a project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    const std::string assetsPath = projectPath + "/Assets";

    BE_LOG_DEBUG(AssetLog, "Scanning assets directory: {}", assetsPath);

    // Check if assets directory exists
    if (!FileSystem::BE_Directory_Exists(assetsPath)) {
        if (!FileSystem::BE_Create_Directory(assetsPath)) {
            BE_LOG_ERROR(AssetLog, "Failed to create assets directory");
            return false;
        }
    }

    // Create asset type directories if they don't exist
    for (std::vector<std::string> assetTypes = {"Meshes", "Textures", "Shaders", "Materials", "Scenes"};
         const auto &type: assetTypes) {
        std::string typePath = assetsPath;
        typePath += '/';
        typePath += type;
        if (!FileSystem::BE_Directory_Exists(typePath)) {
            FileSystem::BE_Create_Directory(typePath);
        }
    }

    // Define the scanning function
    std::function<void(const std::string &)> scanDirectory;

    scanDirectory = [this, &scanDirectory](const std::string &dirPath) {
        for (auto files = FileSystem::BE_Get_Files_In_Directory(dirPath); const auto &file: files) {
            if (std::string ext = FileSystem::BE_Get_File_Extension(file); ext == ".meta") {
                // Skip metadata files
                continue;
            }
            RegisterAsset(file);
        }

        for (auto subdirs = FileSystem::BE_Get_Directories_In_Directory(dirPath); const auto &subdir: subdirs) {
            scanDirectory(subdir);
        }
    };

    // Start scanning from the assets directory
    scanDirectory(assetsPath);

    BE_LOG_INFO(AssetLog, "Asset discovery complete");
    return true;
}

// Register an asset in the database
void AssetDatabase::RegisterAsset(const std::string &assetPath) {
    const std::string assetName = FileSystem::BE_Get_File_Name_Without_Extension(assetPath);
    m_AssetPathRegistry[assetName] = assetPath;
}

// Import an asset from external source
bool AssetDatabase::ImportAsset(const std::string &sourcePath, const std::string &assetType) {
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    const std::string assetsPath = projectPath + "/Assets";
    const std::string fileName = FileSystem::BE_Get_File_Name(sourcePath);
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
    if (!FileSystem::BE_File_Exists(sourcePath)) {
        BE_LOG_ERROR(AssetLog, "Source file not found: {}", sourcePath);
        return false;
    }

    // Read a source file
    const std::vector<uint8_t> data = FileSystem::BE_Read_Binary_File(sourcePath);
    if (data.empty()) {
        BE_LOG_ERROR(AssetLog, "Failed to read source file: {}", sourcePath);
        return false;
    }

    // Write to a target file
    if (!FileSystem::BE_Write_Binary_File(targetPath, data)) {
        BE_LOG_ERROR(AssetLog, "Failed to write target file: {}", targetPath);
        return false;
    }

    // Register the new asset
    RegisterAsset(targetPath);

    // Create metadata
    SaveAssetMetadata(targetPath);

    BE_LOG_INFO(AssetLog, "Successfully loaded asset database");
    return true;
}

// Unload a specific asset from memory
void AssetDatabase::UnloadAsset(const std::string &assetPath) {
    if (const auto it = m_LoadedAssets.find(assetPath); it != m_LoadedAssets.end()) {
        delete it->second;
        BE_LOG_INFO(AssetLog, "Unloaded asset: {}", assetPath);
        m_LoadedAssets.erase(it);
    }
}

// Unload all assets from memory
void AssetDatabase::UnloadAllAssets() {
    for (const auto &asset: m_LoadedAssets | std::views::values) {
        delete asset;
    }
    BE_LOG_INFO(AssetLog, "Unloaded {} assets", m_LoadedAssets.size());
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
    for (const auto &val: m_AssetPathRegistry | std::views::values) {
        if (FileSystem::BE_Get_File_Extension(val) == extension) {
            result.push_back(val);
        }
    }

    return result;
}

// Save metadata for an asset
bool AssetDatabase::SaveAssetMetadata(const std::string &assetPath) {
    // Create metadata JSON
    json metadata;
    metadata["assetId"] = GenerateAssetID();
    metadata["type"] = GetAssetTypeFromExtension(FileSystem::BE_Get_File_Extension(assetPath));
    metadata["importTime"] = std::time(nullptr);

    // Write a metadata file
    const std::string metaPath = assetPath + ".meta";
    return FileSystem::BE_Write_Text_File(metaPath, metadata.dump(4));
}

// Load metadata for an asset
bool AssetDatabase::LoadAssetMetadata(const std::string &metaPath, AssetMetadata &metadata) {
    if (!FileSystem::BE_File_Exists(metaPath)) {
        BE_LOG_ERROR(AssetLog, "Metadata file not found: {}", metaPath);
        return false;
    }

    try {
        std::string content = FileSystem::BE_Read_Text_File(metaPath);
        json j = json::parse(content);

        metadata.uuid = j["uuid"];
        metadata.type = j["type"];
        metadata.name = j["name"];

        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(AssetLog, "Failed to load metadata from '{}': {}", metaPath, e.what());
        return false;
    }
}

// Get asset name from a path
std::string AssetDatabase::GetAssetNameFromPath(const std::string &path) {
    return FileSystem::BE_Get_File_Name_Without_Extension(path);
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
    const std::vector<uint8_t> data = FileSystem::BE_Read_Binary_File(sourcePath);
    if (data.empty()) {
        BE_LOG_ERROR(AssetLog, "Failed to read source file: {}", sourcePath);
        return false;
    }

    // Write to a target file
    if (!FileSystem::BE_Write_Binary_File(targetPath, data)) {
        BE_LOG_ERROR(AssetLog, "Failed to write target file: {}", targetPath);
        return false;
    }
    return true;
}