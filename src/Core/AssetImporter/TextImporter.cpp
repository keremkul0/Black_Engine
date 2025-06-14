#include "TextImporter.h"
#include "Core/Logger/LogMacros.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/ProjectManager/ProjectManager.h"
#include <filesystem>

BE_DEFINE_LOG_CATEGORY(TextImporterLog, "TextImporter");

namespace BlackEngine {

std::vector<std::string> TextImporter::SupportedExtensions() const {
    return {".txt"};
}

bool TextImporter::Import(const ImportContext& ctx) {
    BE_LOG_INFO(TextImporterLog, "Importing text file: {}", ctx.assetPath);
    
    if (ctx.guid.empty()) {
        BE_LOG_ERROR(TextImporterLog, "Cannot import text file with empty GUID");
        return false;
    }
    
    // Get the project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(TextImporterLog, "Cannot import text file: No active project");
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
        BE_LOG_ERROR(TextImporterLog, "Source file does not exist: {}", fullAssetPath);
        return false;
    }
    
    // Create the Library directory if it doesn't exist
    const std::string libraryDir = FileSystem::BE_Combine_Paths(projectPath, "Library");
    if (!FileSystem::BE_Directory_Exists(libraryDir)) {
        if (!FileSystem::BE_Create_Directory(libraryDir)) {
            BE_LOG_ERROR(TextImporterLog, "Failed to create Library directory: {}", libraryDir);
            return false;
        }
    }
    
    // Destination path for the imported binary
    const std::string binaryFilePath = FileSystem::BE_Combine_Paths(libraryDir, ctx.guid + ".bin");
    
    // For text importer, we simply copy the file bytes directly
    if (!FileSystem::BE_Copy_File(fullAssetPath, binaryFilePath)) {
        BE_LOG_ERROR(TextImporterLog, "Failed to copy text file from {} to {}", fullAssetPath, binaryFilePath);
        return false;
    }
    
    // Ensure the binary file exists after copy
    if (!FileSystem::BE_File_Exists(binaryFilePath)) {
        BE_LOG_ERROR(TextImporterLog, "Binary file was not created: {}", binaryFilePath);
        return false;
    }
    
    // Update the binary file's timestamp to the current time to ensure it's newer than the source
    std::error_code ec;
    const auto now = std::filesystem::file_time_type::clock::now();
    std::filesystem::last_write_time(binaryFilePath, now, ec);
    if (ec) {
        BE_LOG_WARNING(TextImporterLog, "Failed to update binary file timestamp: {}", ec.message());
    }
    
    BE_LOG_INFO(TextImporterLog, "Text file imported successfully to {}", binaryFilePath);
    return true;
}

} // namespace BlackEngine
