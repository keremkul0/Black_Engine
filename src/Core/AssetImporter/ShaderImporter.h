#pragma once

#include "ImporterRegistry.h"
#include "Core/AssetImporter/IAssetImporter.h"

namespace BlackEngine {

/**
 * @class ShaderImporter
 * @brief Importer for shader assets (GLSL, HLSL, etc.)
 * 
 * This importer handles shader files by encoding their text content
 * as base64 and storing it in a binary file.
 */
class ShaderImporter final : public IAssetImporter {
public:
    /**
     * @brief Returns the file extensions supported by this importer
     * 
     * @return Vector of supported file extensions
     */
    [[nodiscard]] std::vector<std::string> SupportedExtensions() const override;
    
    /**
     * @brief Imports a shader file
     * 
     * Reads the shader text and encodes it as base64 in the binary file.
     * 
     * @param ctx The import context
     * @return True if import was successful, false otherwise
     */
    bool Import(const ImportContext& ctx) override;

private:
    /**
     * @brief Encodes a string as base64
     * 
     * @param input The string to encode
     * @return The base64-encoded string
     */
    static std::string Base64Encode(const std::string& input);
};

// Register the importer using the registration macro
BE_REGISTER_IMPORTER(ShaderImporter);

} // namespace BlackEngine
