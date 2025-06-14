#pragma once

#include "ImporterRegistry.h"
#include "Core/AssetImporter/IAssetImporter.h"

namespace BlackEngine {

/**
 * @class TextAssetImporter
 * @brief Importer for text assets (TXT, JSON, XML, LUA, etc.)
 * 
 * This importer handles text files by encoding their content
 * as raw text in the binary file.
 */
class TextAssetImporter final : public IAssetImporter {
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
     * Reads the text content and stores it in the binary file.
     * 
     * @param ctx The import context
     * @return True if import was successful, false otherwise
     */
    bool Import(const ImportContext& ctx) override;
};

// Register the importer using the registration macro
BE_REGISTER_IMPORTER(TextAssetImporter);

} // namespace BlackEngine
