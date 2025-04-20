#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Core/FileSystem/FileSystem.h"
#include "Core/Utils/MetaFile.h"

namespace BlackEngine {

/**
 * @class AssetDatabase
 * @brief Manages assets and their metadata in the project
 * 
 * This singleton class handles asset importing, tracking, and metadata management.
 * It maintains a GUID-to-path mapping for all assets in the project.
 */
class AssetDatabase {
public:
    /**
     * @brief Gets the singleton instance of the AssetDatabase
     * @return Reference to the AssetDatabase instance
     */
    static AssetDatabase& GetInstance();

    /**
     * @brief Imports an asset from a source path to the project's Assets directory
     * 
     * @param srcPath The source path of the asset to import
     * @return True if import was successful, false otherwise
     */
    bool ImportAsset(const std::string& srcPath);

    /**
     * @brief Scans the Assets directory and refreshes the asset cache
     * 
     * This method recursively scans the Assets directory, reads all .meta files,
     * updates the internal GUID-to-path mapping, and writes the mapping to 
     * Library/asset_map.json.
     */
    void RefreshAssetCache();

    /**
     * @brief Gets the asset path corresponding to a GUID
     * 
     * @param guid The GUID to look up
     * @return The asset path, or empty string if GUID is not found
     */
    const std::string& GetAssetPath(const std::string& guid) const;

private:
    AssetDatabase();
    ~AssetDatabase() = default;
    
    // Delete copy and move constructors/assignments
    AssetDatabase(const AssetDatabase&) = delete;
    AssetDatabase& operator=(const AssetDatabase&) = delete;
    AssetDatabase(AssetDatabase&&) = delete;
    AssetDatabase& operator=(AssetDatabase&&) = delete;

    /**
     * @brief Detects asset type based on file extension
     * 
     * @param path Path to the asset
     * @return String representing the asset type
     */
    static std::string DetectType(const std::string& path);

    /**
     * @brief Detects appropriate importer based on file extension
     * 
     * @param path Path to the asset
     * @return String representing the importer type
     */
    static std::string DetectImporter(const std::string& path) ;

    /**
     * @brief Loads asset map from Library/asset_map.json
     */
    void LoadAssetMap();

    // GUID to asset path mapping
    std::unordered_map<std::string, std::string> m_GuidToPathMap;
    
    // Flag to track if cache is initialized
    bool m_Initialized;
};

} // namespace BlackEngine
