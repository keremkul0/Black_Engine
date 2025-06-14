#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "IAssetImporter.h"

namespace BlackEngine {

/**
 * @class ImporterRegistry
 * @brief Registry for asset importers
 * 
 * This class manages registration and retrieval of asset importers
 * based on file extensions. It implements the singleton pattern.
 */
class ImporterRegistry {
public:
    /**
     * @brief Gets the singleton instance of the ImporterRegistry
     * @return Reference to the ImporterRegistry instance
     */
    static ImporterRegistry& GetInstance();
    
    /**
     * @brief Registers an importer with the registry
     * 
     * @param importer Pointer to the importer to register
     * @return True if registration was successful, false otherwise
     */
    static bool RegisterImporter(IAssetImporter* importer);
    
    /**
     * @brief Gets an importer for a specific file extension
     * 
     * @param extension The file extension to get an importer for
     * @return Pointer to the appropriate importer, or nullptr if none found
     */
    IAssetImporter* GetImporterForExtension(const std::string& extension);
    
    /**
     * @brief Cleans up all registered importers
     */
    void Cleanup();

    // Delete copy and move constructors/assignments
    ImporterRegistry(const ImporterRegistry&) = delete;
    ImporterRegistry& operator=(const ImporterRegistry&) = delete;
    ImporterRegistry(ImporterRegistry&&) = delete;
    ImporterRegistry& operator=(ImporterRegistry&&) = delete;

private:
    ImporterRegistry() = default;
    ~ImporterRegistry();

    // Map of file extensions to importers - using raw pointers but ownership in m_RegisteredImporters
    std::unordered_map<std::string, IAssetImporter*> m_ExtensionToImporterMap;
    
    // Collection of all registered importers for cleanup - owns the memory
    std::vector<std::unique_ptr<IAssetImporter>> m_RegisteredImporters;
};

/**
 * @brief Macro to register an importer class
 * 
 * This macro creates a static variable that registers an instance of
 * the importer class with the ImporterRegistry when the program starts.
 */
#define BE_REGISTER_IMPORTER(CLASS) \
    static bool _##CLASS##_registered = (ImporterRegistry::RegisterImporter(new CLASS()), true);

} // namespace BlackEngine
