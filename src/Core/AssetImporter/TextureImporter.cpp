#include "TextureImporter.h"
#include "Core/Logger/LogMacros.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/ProjectManager/ProjectManager.h"

BE_DEFINE_LOG_CATEGORY(TextureImporterLog, "TextureImporter");

namespace BlackEngine {

std::vector<std::string> TextureImporter::SupportedExtensions() const {
    return {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd"};
}

bool TextureImporter::Import(const ImportContext& ctx) {
    BE_LOG_INFO(TextureImporterLog, "Importing texture: {}", ctx.assetPath);
    
    if (ctx.guid.empty()) {
        BE_LOG_ERROR(TextureImporterLog, "Cannot import texture with empty GUID");
        return false;
    }
    
    // Get the project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(TextureImporterLog, "Cannot import texture: No active project");
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
        BE_LOG_ERROR(TextureImporterLog, "Source file does not exist: {}", fullAssetPath);
        return false;
    }
    
    // Create the Library directory if it doesn't exist
    const std::string libraryDir = FileSystem::BE_Combine_Paths(projectPath, "Library");
    if (!FileSystem::BE_Directory_Exists(libraryDir)) {
        if (!FileSystem::BE_Create_Directory(libraryDir)) {
            BE_LOG_ERROR(TextureImporterLog, "Failed to create Library directory: {}", libraryDir);
            return false;
        }
    }
    
    // Destination path for the imported binary
    const std::string dstPath = FileSystem::BE_Combine_Paths(libraryDir, ctx.guid + ".bin");
    
    // For texture importer, we simply copy the file bytes directly
    if (!FileSystem::BE_Copy_File(fullAssetPath, dstPath)) {
        BE_LOG_ERROR(TextureImporterLog, "Failed to copy texture from {} to {}", fullAssetPath, dstPath);
        return false;
    }
    
    BE_LOG_INFO(TextureImporterLog, "Texture imported successfully to {}", dstPath);
    return true;
}

} // namespace BlackEngine
