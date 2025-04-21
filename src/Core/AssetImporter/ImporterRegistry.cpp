#include "ImporterRegistry.h"
#include "Core/Logger/LogMacros.h"
#include <algorithm>

// Define and register the ImporterRegistry log category
BE_DEFINE_LOG_CATEGORY(ImporterRegistryLog, "ImporterRegistry");

namespace BlackEngine {

ImporterRegistry& ImporterRegistry::GetInstance() {
    static ImporterRegistry instance;
    return instance;
}

ImporterRegistry::~ImporterRegistry() {
    Cleanup();
}

bool ImporterRegistry::RegisterImporter(IAssetImporter* importer) {
    if (!importer) {
        BE_LOG_ERROR(ImporterRegistryLog, "Cannot register null importer");
        return false;
    }
    
    ImporterRegistry& registry = GetInstance();
    
    // Get the supported extensions from the importer
    std::vector<std::string> extensions = importer->SupportedExtensions();
    
    if (extensions.empty()) {
        BE_LOG_WARNING(ImporterRegistryLog, "Importer does not support any extensions");
        return false;
    }
    
    // Register the importer for each extension
    for (const auto& ext : extensions) {
        // Make sure the extension starts with a dot
        std::string extension = ext;
        if (!extension.empty() && extension[0] != '.') {
            extension = "." + extension;
        }
          // Convert to lowercase for case-insensitive matching
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        // Check if this extension is already registered
        if (registry.m_ExtensionToImporterMap.contains(extension)) {
            BE_LOG_WARNING(ImporterRegistryLog, "Extension '{}' already has a registered importer, overriding", extension);
        }
        
        // Register the importer for this extension
        registry.m_ExtensionToImporterMap[extension] = importer;
        BE_LOG_INFO(ImporterRegistryLog, "Registered importer for extension '{}'", extension);
    }
    
    // Add the importer to the registered importers collection for memory management
    registry.m_RegisteredImporters.push_back(std::unique_ptr<IAssetImporter>(importer));
    
    return true;
}

IAssetImporter* ImporterRegistry::GetImporterForExtension(const std::string& extension) {
    // Make sure the extension starts with a dot
    std::string ext = extension;
    if (!ext.empty() && ext[0] != '.') {
        ext = "." + ext;
    }
      // Convert to lowercase for case-insensitive matching
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Look up the importer
    auto it = m_ExtensionToImporterMap.find(ext);
    if (it != m_ExtensionToImporterMap.end()) {
        return it->second;
    }
    
    BE_LOG_WARNING(ImporterRegistryLog, "No importer found for extension '{}'", ext);
    return nullptr;
}

void ImporterRegistry::Cleanup() {
    BE_LOG_INFO(ImporterRegistryLog, "Cleaning up {} registered importers", m_RegisteredImporters.size());
    
    // Clear the extension map
    m_ExtensionToImporterMap.clear();
    
    // Clear the registered importers (which will delete them)
    m_RegisteredImporters.clear();
}

} // namespace BlackEngine
