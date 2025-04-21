#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace BlackEngine {

/**
 * @struct ImportContext
 * @brief Contains context information for asset import operations
 * 
 * This structure holds all necessary information about an asset that
 * is being imported, including its GUID, path, and import settings.
 */
struct ImportContext {
    /**
     * @brief The GUID of the asset being imported
     */
    std::string guid;
    
    /**
     * @brief The path to the asset being imported
     */
    std::string assetPath;
    
    /**
     * @brief Custom import settings for the asset
     */
    nlohmann::json importSettings;
};

} // namespace BlackEngine
