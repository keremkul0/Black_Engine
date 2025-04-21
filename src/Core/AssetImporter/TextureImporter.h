#pragma once

#include "ImporterRegistry.h"
#include "Core/AssetImporter/IAssetImporter.h"

namespace BlackEngine {

/**
 * @class TextureImporter
 * @brief Importer for texture assets (PNG, JPG, etc.)
 * 
 * This importer handles texture files by copying the raw file bytes
 * to the Library directory as binary data.
 */
class TextureImporter : public IAssetImporter {
public:
    /**
     * @brief Returns the file extensions supported by this importer
     * 
     * @return Vector of supported file extensions
     */
    std::vector<std::string> SupportedExtensions() const override;
    
    /**
     * @brief Imports a texture file
     * 
     * Simply copies the raw file bytes to Library/{guid}.bin
     * 
     * @param ctx The import context
     * @return True if import was successful, false otherwise
     */
    bool Import(const ImportContext& ctx) override;
};

// Register the importer using the registration macro
BE_REGISTER_IMPORTER(TextureImporter);

} // namespace BlackEngine
