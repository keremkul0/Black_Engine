#pragma once
#include <string>
#include <nlohmann/json.hpp>

// Forward declaration of AssetDatabase to avoid circular dependency
namespace BlackEngine {
class AssetDatabase;
}

using json = nlohmann::json;

class MetaFile {
public:
    /**
     * Creates a new meta file for an asset with a unique GUID.
     * @param assetPath Path to the asset
     * @param type Asset type (optional)
     * @param importer Importer type (optional)
     * @return True if the meta file was created successfully
     */
    static bool Create(const std::string& assetPath, const std::string& type = "", 
                       const std::string& importer = "");
    
    /**
     * Loads the meta file for an asset.
     * @param assetPath Path to the asset
     * @return The meta file JSON content or empty JSON if not found
     */
    static json Load(const std::string& assetPath);
    
    /**
     * Saves the meta file for an asset.
     * @param assetPath Path to the asset
     * @param metaData JSON data to save in the meta file
     * @return True if the meta file was saved successfully
     */
    static bool Save(const std::string& assetPath, const json& metaData);

private:
    /**
     * Gets the meta file path for an asset.
     * @param assetPath Path to the asset
     * @return The path to the meta file
     */
    static std::string GetMetaFilePath(const std::string& assetPath);
};
