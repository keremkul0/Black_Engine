#pragma once

#include "ImporterRegistry.h"
#include "Core/AssetImporter/IAssetImporter.h"

namespace BlackEngine {

/**
 * @class TextImporter
 * @brief Importer for text assets (.txt files)
 * 
 * This importer handles text files by copying their content
 * to the Library directory as binary data.
 */
class TextImporter final : public IAssetImporter {
public:
    /**
     * @brief Returns the file extensions supported by this importer
     * 
     * @return Vector of supported file extensions
     */
    [[nodiscard]] std::vector<std::string> SupportedExtensions() const override;
    
    /**
     * @brief Imports a text file
     * 
     * Copies the text file content to Library/{guid}.bin
     * 
     * @param ctx The import context
     * @return True if import was successful, false otherwise
     */
    bool Import(const ImportContext& ctx) override;
};

// Register the importer using the registration macro
// BE_REGISTER_IMPORTER(TextImporter);

} // namespace BlackEngine
