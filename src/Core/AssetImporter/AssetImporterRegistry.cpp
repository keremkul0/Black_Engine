#include "AssetImporterRegistry.h"
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "ShaderImporter.h"
#include "TextAssetImporter.h"
#include "LuaScriptImporter.h"
#include "ImporterRegistry.h"
#include "Core/Logger/LogMacros.h"

BE_DEFINE_LOG_CATEGORY(AssetImporterRegistryLog, "AssetImporterRegistry");

namespace BlackEngine {

void InitializeAssetImporters() {
    BE_LOG_INFO(AssetImporterRegistryLog, "Initializing asset importers");
    
    // Get the importer registry singleton
    ImporterRegistry& registry = ImporterRegistry::GetInstance();

    // Register all importers directly
    ImporterRegistry::RegisterImporter(new TextureImporter());
    ImporterRegistry::RegisterImporter(new MeshImporter());
    ImporterRegistry::RegisterImporter(new ShaderImporter());
    ImporterRegistry::RegisterImporter(new TextAssetImporter());
    ImporterRegistry::RegisterImporter(new LuaScriptImporter());
    
    BE_LOG_INFO(AssetImporterRegistryLog, "Asset importers initialized");
}

}
