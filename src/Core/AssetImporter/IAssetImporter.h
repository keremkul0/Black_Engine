#pragma once

#include <string>
#include <vector>
#include "ImportContext.h"

namespace BlackEngine {

/**
 * @class IAssetImporter
 * @brief Interface for asset importers
 * 
 * This interface defines the contract for asset importers that convert
 * source assets into runtime binary representations.
 */
class IAssetImporter {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~IAssetImporter() = default;
    
    /**
     * @brief Returns a list of file extensions supported by this importer
     * 
     * @return Vector of supported file extensions (e.g., ".png", ".obj")
     */
    [[nodiscard]] virtual std::vector<std::string> SupportedExtensions() const = 0;
    
    /**
     * @brief Imports an asset and generates a binary representation
     * 
     * @param ctx The import context containing asset information
     * @return True if import was successful, false otherwise
     */
    virtual bool Import(const ImportContext& ctx) = 0;
};

} // namespace BlackEngine
