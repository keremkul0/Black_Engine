#include "TextAssetImporter.h"
#include "Core/Logger/LogMacros.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/ProjectManager/ProjectManager.h"
#include <fstream>
#include <string>
#include <filesystem>

BE_DEFINE_LOG_CATEGORY(TextAssetImporterLog, "TextAssetImporter");

namespace BlackEngine {

std::vector<std::string> TextAssetImporter::SupportedExtensions() const {
    return {".txt", ".json", ".xml", ".csv", ".md"};
}

bool TextAssetImporter::Import(const ImportContext& ctx) {
    BE_LOG_INFO(TextAssetImporterLog, "Importing text asset: {}", ctx.assetPath);
    
    if (ctx.guid.empty()) {
        BE_LOG_ERROR(TextAssetImporterLog, "Cannot import text asset with empty GUID");
        return false;
    }
    
    // Get the project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(TextAssetImporterLog, "Cannot import text asset: No active project");
        return false;
    }
    
    // Full path to the asset
    std::string fullAssetPath;
    if (ctx.assetPath.size() > 1 && (ctx.assetPath[0] == '/' || ctx.assetPath[0] == '\\' || 
        (ctx.assetPath.size() > 2 && ctx.assetPath[1] == ':' && (ctx.assetPath[2] == '/' || ctx.assetPath[2] == '\\')))) {
        // Check if path is already absolute
        fullAssetPath = ctx.assetPath;
    } else {
        fullAssetPath = FileSystem::BE_Combine_Paths(projectPath, ctx.assetPath);
    }
    
    // Check if the source file exists
    if (!FileSystem::BE_File_Exists(fullAssetPath)) {
        BE_LOG_ERROR(TextAssetImporterLog, "Source file does not exist: {}", fullAssetPath);
        return false;
    }
    
    // Create the Library directory if it doesn't exist
    const std::string libraryDir = FileSystem::BE_Combine_Paths(projectPath, "Library");
    if (!FileSystem::BE_Directory_Exists(libraryDir)) {
        if (!FileSystem::BE_Create_Directory(libraryDir)) {
            BE_LOG_ERROR(TextAssetImporterLog, "Failed to create Library directory: {}", libraryDir);
            return false;
        }
    }
    
    // Destination path for the imported binary
    const std::string dstPath = FileSystem::BE_Combine_Paths(libraryDir, ctx.guid + ".bin");
    
    // Read the text content
    std::string textContent;
    try {
        textContent = FileSystem::BE_Read_Text_File(fullAssetPath);
    } catch (const std::exception& e) {
        BE_LOG_ERROR(TextAssetImporterLog, "Failed to read text content: {}", e.what());
        return false;
    }
    
    // Write the text content to the binary file
    std::ofstream outFile(dstPath, std::ios::binary);
    if (!outFile.is_open()) {
        BE_LOG_ERROR(TextAssetImporterLog, "Failed to open output file for writing: {}", dstPath);
        return false;
    }
      outFile.write(textContent.c_str(), static_cast<std::streamsize>(textContent.size()));
    outFile.close();
    
    // Update the binary file's timestamp to ensure it's newer than the source
    std::error_code ec;
    const auto now = std::filesystem::file_time_type::clock::now();
    std::filesystem::last_write_time(dstPath, now, ec);
    if (ec) {
        BE_LOG_WARNING(TextAssetImporterLog, "Failed to update binary file timestamp: {}", ec.message());
    }
    
    BE_LOG_INFO(TextAssetImporterLog, "Text asset imported successfully to {}", dstPath);
    return true;
}

} // namespace BlackEngine
