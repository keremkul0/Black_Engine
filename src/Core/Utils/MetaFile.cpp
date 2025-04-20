#include "MetaFile.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Logger/LogMacros.h"

// Define log category for MetaFile
BE_DEFINE_LOG_CATEGORY(MetaFileLog, "MetaFile");

/**
 * Creates a new meta file for an asset with a unique GUID.
 * @param assetPath Path to the asset
 * @param type Asset type (optional)
 * @param importer Importer type (optional)
 * @return True if the meta file was created successfully
 */
bool MetaFile::Create(const std::string& assetPath, const std::string& type, 
                     const std::string& importer) {
    // Make sure the asset exists
    if (!FileSystem::BE_File_Exists(assetPath)) {
        BE_LOG_ERROR(MetaFileLog, "Cannot create meta file for non-existent asset: {}", assetPath);
        return false;
    }
    
    // Check if meta file already exists
    std::string metaFilePath = GetMetaFilePath(assetPath);
    if (FileSystem::BE_File_Exists(metaFilePath)) {
        BE_LOG_WARNING(MetaFileLog, "Meta file already exists for asset: {}", assetPath);
        return true;
    }
    
    // Create meta data with a new GUID
    json metaData;
    metaData["guid"] = GuidUtils::GenerateGuid();
    metaData["type"] = type;
    metaData["importer"] = importer;
    metaData["settings"] = json::object();
    
    // Save the meta file
    return Save(assetPath, metaData);
}

/**
 * Loads the meta file for an asset.
 * @param assetPath Path to the asset
 * @return The meta file JSON content or empty JSON if not found
 */
json MetaFile::Load(const std::string& assetPath) {
    const std::string metaFilePath = GetMetaFilePath(assetPath);
    
    // Check if meta file exists
    if (!FileSystem::BE_File_Exists(metaFilePath)) {
        BE_LOG_WARNING(MetaFileLog, "Meta file not found for asset: {}", assetPath);
        return json::object();
    }
    
    // Read and parse meta file
    try {
        std::string metaContent = FileSystem::BE_Read_Text_File(metaFilePath);
        return json::parse(metaContent);
    } catch (const std::exception& e) {
        BE_LOG_ERROR(MetaFileLog, "Error parsing meta file for asset {}: {}", assetPath, e.what());
        return json::object();
    }
}

/**
 * Saves the meta file for an asset.
 * @param assetPath Path to the asset
 * @param metaData JSON data to save in the meta file
 * @return True if the meta file was saved successfully
 */
bool MetaFile::Save(const std::string& assetPath, const json& metaData) {
    std::string metaFilePath = GetMetaFilePath(assetPath);
    
    // Generate formatted JSON string

    // Write to file
    if (const std::string jsonString = metaData.dump(4); !FileSystem::BE_Write_Text_File(metaFilePath, jsonString)) {
        BE_LOG_ERROR(MetaFileLog, "Failed to write meta file: {}", metaFilePath);
        return false;
    }
    
    BE_LOG_INFO(MetaFileLog, "Created meta file for asset: {}", assetPath);
    return true;
}

/**
 * Gets the meta file path for an asset.
 * @param assetPath Path to the asset
 * @return The path to the meta file
 */
std::string MetaFile::GetMetaFilePath(const std::string& assetPath) {
    return assetPath + ".meta";
}
