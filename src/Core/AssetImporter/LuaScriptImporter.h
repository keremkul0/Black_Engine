#pragma once

#include "ImporterRegistry.h"
#include "Core/AssetImporter/IAssetImporter.h"

namespace BlackEngine {

/**
 * @class LuaScriptImporter
 * @brief Importer for Lua script assets (.lua)
 * 
 * This importer handles Lua script files by storing them in the Library directory.
 * It supports the game engine's internal scripting language.
 */
class LuaScriptImporter final : public IAssetImporter {
public:
    /**
     * @brief Returns the file extensions supported by this importer
     * 
     * @return Vector of supported file extensions
     */
    [[nodiscard]] std::vector<std::string> SupportedExtensions() const override;
    
    /**
     * @brief Imports a Lua script file
     * 
     * @param ctx The import context
     * @return True if import was successful, false otherwise
     */
    bool Import(const ImportContext& ctx) override;
};

// Register the importer using the registration macro
BE_REGISTER_IMPORTER(LuaScriptImporter);

} // namespace BlackEngine
