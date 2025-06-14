#pragma once

namespace BlackEngine {
    /**
     * @brief Initializes and registers all asset importers
     * 
     * This function ensures all importers are properly registered with the ImporterRegistry.
     * It is called during the engine's initialization process.
     */
    void InitializeAssetImporters();
}
