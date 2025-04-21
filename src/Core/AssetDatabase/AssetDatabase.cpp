#include "AssetDatabase.h"
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/Utils/GuidUtils.h"
#include "Core/Logger/LogMacros.h"
#include "Core/AssetImporter/ImporterRegistry.h"
#include "Core/AssetImporter/ImportContext.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// Define and register the AssetDatabase log category
BE_DEFINE_LOG_CATEGORY(AssetDatabaseLog, "AssetDatabase");

namespace BlackEngine {

AssetDatabase& AssetDatabase::GetInstance() {
    static AssetDatabase instance;
    return instance;
}

AssetDatabase::AssetDatabase() : m_Initialized(false) {
    BE_LOG_INFO(AssetDatabaseLog, "AssetDatabase initialized");
    LoadAssetMap();
}

void AssetDatabase::LoadAssetMap() {
    // Clear the current mapping
    m_GuidToPathMap.clear();
    
    // Get the path to the asset map
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_WARNING(AssetDatabaseLog, "Cannot load asset map: No active project");
        return;
    }
    
    const std::string assetMapPath = FileSystem::BE_Combine_Paths(projectPath, "Library/asset_map.json");
    
    // If asset map doesn't exist, don't try to load it
    if (!FileSystem::BE_File_Exists(assetMapPath)) {
        BE_LOG_INFO(AssetDatabaseLog, "Asset map not found at {}, will be created on next refresh", assetMapPath);
        return;
    }
    
    // Load and parse the asset map
    try {
        nlohmann::json assetMap = FileSystem::BE_Read_JSON(assetMapPath);
        
        // Populate the map from JSON
        for (auto& [guid, path] : assetMap.items()) {
            m_GuidToPathMap[guid] = path;
        }
        
        BE_LOG_INFO(AssetDatabaseLog, "Loaded asset map with {} entries", m_GuidToPathMap.size());
        m_Initialized = true;
    } catch (const std::exception& e) {
        BE_LOG_ERROR(AssetDatabaseLog, "Error loading asset map: {}", e.what());
    }
}

bool AssetDatabase::ImportAsset(const std::string& srcPath) {
    if (!FileSystem::BE_File_Exists(srcPath)) {
        BE_LOG_ERROR(AssetDatabaseLog, "Source file does not exist: {}", srcPath);
        return false;
    }
    
    // Get project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot import asset: No active project");
        return false;
    }
    
    // Extract the filename from the source path
    const std::string fileName = FileSystem::BE_Get_File_Name(srcPath);
      // Create the destination path in the Assets directory
    const std::string assetsDir = FileSystem::BE_Combine_Paths(projectPath, "Assets");
    const std::string dstPath = FileSystem::BE_Combine_Paths(assetsDir, fileName);
    
    // Check if the destination file and meta file already exist (reimport case)
    const std::string metaPath = dstPath + ".meta";
    std::string guid;
    bool metaExists = FileSystem::BE_File_Exists(metaPath);
    
    // If this is a reimport, read the existing GUID first
    if (metaExists) {
        // Read the existing meta file to get the GUID before overwriting the file
        nlohmann::json metaData = MetaFile::Load(dstPath);
        if (!metaData.empty() && metaData.contains("guid")) {
            guid = metaData["guid"];
            BE_LOG_INFO(AssetDatabaseLog, "Reimporting asset, preserving existing GUID: {}", guid);
        } else {
            BE_LOG_WARNING(AssetDatabaseLog, "Existing meta file found but GUID couldn't be read");
            metaExists = false; // Treat as new import if GUID can't be read
        }
    }
    
    // Copy the file
    if (!FileSystem::BE_Copy_File(srcPath, dstPath)) {
        BE_LOG_ERROR(AssetDatabaseLog, "Failed to copy asset from {} to {}", srcPath, dstPath);
        return false;
    }
    
    if (!metaExists) {
        // Create new meta file if it doesn't exist
        const std::string assetType = DetectType(dstPath);
        const std::string importerType = DetectImporter(dstPath);
        
        BE_LOG_INFO(AssetDatabaseLog, "Creating new meta file for {} with type {} and importer {}", 
                    dstPath, assetType, importerType);
        
        if (!MetaFile::Create(dstPath, assetType, importerType)) {
            BE_LOG_ERROR(AssetDatabaseLog, "Failed to create meta file for {}", dstPath);
            return false;
        }
        
        // Load the newly created meta file to get the GUID
        nlohmann::json metaData = MetaFile::Load(dstPath);
        if (metaData.empty() || !metaData.contains("guid")) {
            BE_LOG_ERROR(AssetDatabaseLog, "Failed to read GUID from meta file for {}", dstPath);
            return false;
        }
        
        guid = metaData["guid"];
    } else {
        // For reimports, we need to ensure the meta file still exists after the file copy
        // This is a safety check in case the copy process somehow affected the meta file
        if (!FileSystem::BE_File_Exists(metaPath)) {
            BE_LOG_ERROR(AssetDatabaseLog, "Meta file was lost during reimport: {}", metaPath);
            return false;
        }
        
        BE_LOG_INFO(AssetDatabaseLog, "Using existing meta file with GUID {} for {}", guid, dstPath);
    }
    
    // Update the GUID-to-path mapping
    const std::string relativePath = "Assets/" + fileName;
    m_GuidToPathMap[guid] = relativePath;
    
    // Save the updated asset map
    RefreshAssetCache();
    
    BE_LOG_INFO(AssetDatabaseLog, "Asset imported successfully: {} -> {} (GUID: {})", 
                srcPath, relativePath, guid);
    
    return true;
}

void AssetDatabase::RefreshAssetCache() {
    // Get project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot refresh asset cache: No active project");
        return;
    }
    
    // Clear the current mapping
    m_GuidToPathMap.clear();
    
    // Get the path to the Assets directory
    const std::string assetsDir = FileSystem::BE_Combine_Paths(projectPath, "Assets");
    
    // Check if Assets directory exists
    if (!FileSystem::BE_Directory_Exists(assetsDir)) {
        BE_LOG_WARNING(AssetDatabaseLog, "Assets directory does not exist: {}", assetsDir);
        return;
    }
    
    // Recursively scan the Assets directory
    BE_LOG_INFO(AssetDatabaseLog, "Scanning Assets directory: {}", assetsDir);
    
    try {
        // Use C++17 filesystem to recursively iterate through the directory
        for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
            if (fs::is_regular_file(entry) && entry.path().extension() != ".meta") {
                // For each non-meta file, check if it has a meta file
                const std::string assetPath = entry.path().string();
                const std::string metaPath = assetPath + ".meta";
                
                if (fs::exists(metaPath)) {
                    // Read the meta file to get the GUID
                    nlohmann::json metaData = MetaFile::Load(assetPath);
                    if (!metaData.empty() && metaData.contains("guid")) {
                        const std::string guid = metaData["guid"];
                        
                        // Calculate the relative path from the project root
                        std::string relativePath = assetPath.substr(projectPath.length() + 1);
                        // Replace backslashes with forward slashes for consistency
                        std::ranges::replace(relativePath, '\\', '/');
                        
                        // Add to the mapping
                        m_GuidToPathMap[guid] = relativePath;
                        BE_LOG_DEBUG(AssetDatabaseLog, "Added to asset map: {} -> {}", guid, relativePath);
                    } else {
                        BE_LOG_WARNING(AssetDatabaseLog, "Meta file for {} does not contain a valid GUID", assetPath);
                    }
                } else {
                    // Create a meta file if one doesn't exist
                    const std::string assetType = DetectType(assetPath);
                    const std::string importerType = DetectImporter(assetPath);
                    
                    BE_LOG_INFO(AssetDatabaseLog, "Creating missing meta file for {} with type {} and importer {}", 
                                assetPath, assetType, importerType);
                    
                    if (MetaFile::Create(assetPath, assetType, importerType)) {
                        // Read the newly created meta file to get the GUID
                        nlohmann::json metaData = MetaFile::Load(assetPath);
                        if (!metaData.empty() && metaData.contains("guid")) {
                            const std::string guid = metaData["guid"];
                            
                            // Calculate the relative path from the project root
                            std::string relativePath = assetPath.substr(projectPath.length() + 1);
                            // Replace backslashes with forward slashes for consistency
                            std::ranges::replace(relativePath, '\\', '/');
                            
                            // Add to the mapping
                            m_GuidToPathMap[guid] = relativePath;
                            BE_LOG_DEBUG(AssetDatabaseLog, "Added to asset map: {} -> {}", guid, relativePath);
                        }
                    } else {
                        BE_LOG_ERROR(AssetDatabaseLog, "Failed to create meta file for {}", assetPath);
                    }
                }
            }
        }
        
        // Create Library directory if it doesn't exist
        const std::string libraryDir = FileSystem::BE_Combine_Paths(projectPath, "Library");
        if (!FileSystem::BE_Directory_Exists(libraryDir)) {
            if (!FileSystem::BE_Create_Directory(libraryDir)) {
                BE_LOG_ERROR(AssetDatabaseLog, "Failed to create Library directory: {}", libraryDir);
                return;
            }
        }
        
        // Save the mapping to asset_map.json
        const std::string assetMapPath = FileSystem::BE_Combine_Paths(libraryDir, "asset_map.json");
        
        // Convert the map to JSON
        nlohmann::json assetMap = nlohmann::json::object();
        for (const auto& [guid, path] : m_GuidToPathMap) {
            assetMap[guid] = path;
        }
        
        // Write to file
        if (FileSystem::BE_Write_JSON(assetMapPath, assetMap)) {
            BE_LOG_INFO(AssetDatabaseLog, "Asset map with {} entries saved to {}", 
                        m_GuidToPathMap.size(), assetMapPath);
            m_Initialized = true;
        } else {
            BE_LOG_ERROR(AssetDatabaseLog, "Failed to save asset map to {}", assetMapPath);
        }
    } catch (const std::exception& e) {
        BE_LOG_ERROR(AssetDatabaseLog, "Error refreshing asset cache: {}", e.what());
    }
}

const std::string& AssetDatabase::GetAssetPath(const std::string& guid) const {
    static const std::string emptyString;
    
    auto it = m_GuidToPathMap.find(guid);
    if (it != m_GuidToPathMap.end()) {
        return it->second;
    }
    
    BE_LOG_WARNING(AssetDatabaseLog, "Asset with GUID {} not found", guid);
    return emptyString;
}

bool AssetDatabase::Reimport(const std::string& guid) const {
    BE_LOG_INFO(AssetDatabaseLog, "Reimporting asset with GUID: {}", guid);
    
    // Get the asset path from the GUID
    const std::string& assetPath = GetAssetPath(guid);
    if (assetPath.empty()) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot reimport: Asset with GUID {} not found", guid);
        return false;
    }
    
    // Get the project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot reimport asset: No active project");
        return false;
    }
    
    // Calculate full asset path
    const std::string fullAssetPath = FileSystem::BE_Combine_Paths(projectPath, assetPath);
    if (!FileSystem::BE_File_Exists(fullAssetPath)) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot reimport: Asset file not found at {}", fullAssetPath);
        return false;
    }
    
    // Get the file extension to find the appropriate importer
    std::string extension = FileSystem::BE_Get_File_Extension(fullAssetPath);
    
    // Find the appropriate importer based on the file extension
    IAssetImporter* importer = ImporterRegistry::GetInstance().GetImporterForExtension(extension);
    if (!importer) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot reimport: No importer found for extension '{}'", extension);
        return false;
    }
    
    // Load asset meta file to get import settings
    nlohmann::json metaData = MetaFile::Load(fullAssetPath);
    if (metaData.empty()) {
        BE_LOG_ERROR(AssetDatabaseLog, "Cannot reimport: Failed to load meta file for {}", fullAssetPath);
        return false;
    }
    
    // Extract import settings (if any)
    nlohmann::json importSettings;
    if (metaData.contains("importSettings")) {
        importSettings = metaData["importSettings"];
    }
    
    // Create import context
    ImportContext ctx;
    ctx.guid = guid;
    ctx.assetPath = fullAssetPath;
    ctx.importSettings = importSettings;
    
    // Perform the import
    BE_LOG_INFO(AssetDatabaseLog, "Using {} to reimport {}", metaData.value("importer", "UnknownImporter"), assetPath);
    if (!importer->Import(ctx)) {
        BE_LOG_ERROR(AssetDatabaseLog, "Failed to reimport asset {}", assetPath);
        return false;
    }
    
    // Asset import was successful, refresh the cache
    BE_LOG_INFO(AssetDatabaseLog, "Successfully reimported {}", assetPath);
    return true;
}

std::string AssetDatabase::DetectType(const std::string& path) {
    // Get the file extension
    std::string extension = FileSystem::BE_Get_File_Extension(path);
    std::ranges::transform(extension, extension.begin(), ::tolower);
    
    // Map common extensions to asset types
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || 
        extension == ".tga" || extension == ".bmp" || extension == ".psd") {
        return "Texture";
    } else if (extension == ".fbx" || extension == ".obj" || extension == ".blend" || 
               extension == ".dae" || extension == ".3ds") {
        return "Model";
    } else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg") {
        return "Audio";
    } else if (extension == ".mp4" || extension == ".avi" || extension == ".mov") {
        return "Video";
    } else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" || 
               extension == ".comp" || extension == ".geom") {
        return "Shader";
    } else if (extension == ".ttf" || extension == ".otf") {
        return "Font";
    } else if (extension == ".txt" || extension == ".json" || extension == ".xml" || 
               extension == ".csv" || extension == ".md") {
        return "TextAsset";
    } else if (extension == ".mat") {
        return "Material";
    } else if (extension == ".prefab") {
        return "Prefab";
    } else if (extension == ".scene") {
        return "Scene";
    }
    
    // Default type for unknown extensions
    return "GenericAsset";
}

std::string AssetDatabase::DetectImporter(const std::string& path) {
    // Get the file extension
    std::string extension = FileSystem::BE_Get_File_Extension(path);
    std::ranges::transform(extension, extension.begin(), ::tolower);
    
    // Map common extensions to importers
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || 
        extension == ".tga" || extension == ".bmp" || extension == ".psd") {
        return "TextureImporter";
    } else if (extension == ".fbx" || extension == ".obj" || extension == ".blend" || 
               extension == ".dae" || extension == ".3ds") {
        return "ModelImporter";
    } else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg") {
        return "AudioImporter";
    } else if (extension == ".mp4" || extension == ".avi" || extension == ".mov") {
        return "VideoImporter";
    } else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" || 
               extension == ".comp" || extension == ".geom") {
        return "ShaderImporter";
    } else if (extension == ".ttf" || extension == ".otf") {
        return "FontImporter";
    } else if (extension == ".txt" || extension == ".json" || extension == ".xml" || 
               extension == ".csv" || extension == ".md") {
        return "TextAssetImporter";
    } else if (extension == ".mat") {
        return "MaterialImporter";
    } else if (extension == ".prefab") {
        return "PrefabImporter";
    } else if (extension == ".scene") {
        return "SceneImporter";
    }
    
    // Default importer for unknown extensions
    return "DefaultImporter";
}

} // namespace BlackEngine
