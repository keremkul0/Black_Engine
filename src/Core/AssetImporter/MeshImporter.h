#pragma once

#include "ImporterRegistry.h"
#include "Core/AssetImporter/IAssetImporter.h"

namespace BlackEngine {

/**
 * @class MeshImporter
 * @brief Importer for mesh assets (OBJ, FBX, etc.)
 * 
 * This importer handles mesh files by parsing basic geometry data
 * and converting it to a binary format.
 */
class MeshImporter final : public IAssetImporter {
public:
    /**
     * @brief Returns the file extensions supported by this importer
     * 
     * @return Vector of supported file extensions
     */
    [[nodiscard]] std::vector<std::string> SupportedExtensions() const override;
    
    /**
     * @brief Imports a mesh file
     * 
     * Parses the OBJ file format and converts vertices to binary data.
     * 
     * @param ctx The import context
     * @return True if import was successful, false otherwise
     */
    bool Import(const ImportContext& ctx) override;

private:
    /**
     * @brief Handles specifically OBJ file format
     * 
     * @param objFilePath Path to the OBJ file
     * @param outputPath Path to write the binary output
     * @return True if successful, false otherwise
     */
    static bool ImportOBJ(const std::string& objFilePath, const std::string& outputPath);
    
    /**
     * @brief Writes a vector of floats to a binary file
     * 
     * @param filePath Path to write to
     * @param data Vector of float data to write
     * @return True if successful, false otherwise
     */
    static bool WriteBinaryData(const std::string& filePath, const std::vector<float>& data);
};

// Register the importer using the registration macro
BE_REGISTER_IMPORTER(MeshImporter);

} // namespace BlackEngine
