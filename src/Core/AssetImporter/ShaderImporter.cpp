#include "ShaderImporter.h"
#include "Core/Logger/LogMacros.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/ProjectManager/ProjectManager.h"
#include <fstream>
#include <vector>
#include <string>

BE_DEFINE_LOG_CATEGORY(ShaderImporterLog, "ShaderImporter");

namespace BlackEngine {

std::vector<std::string> ShaderImporter::SupportedExtensions() const {
    return {".glsl", ".vert", ".frag", ".comp", ".geom"};
}

bool ShaderImporter::Import(const ImportContext& ctx) {
    BE_LOG_INFO(ShaderImporterLog, "Importing shader: {}", ctx.assetPath);
    
    if (ctx.guid.empty()) {
        BE_LOG_ERROR(ShaderImporterLog, "Cannot import shader with empty GUID");
        return false;
    }
    
    // Get the project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(ShaderImporterLog, "Cannot import shader: No active project");
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
        BE_LOG_ERROR(ShaderImporterLog, "Source file does not exist: {}", fullAssetPath);
        return false;
    }
    
    // Create the Library directory if it doesn't exist
    const std::string libraryDir = FileSystem::BE_Combine_Paths(projectPath, "Library");
    if (!FileSystem::BE_Directory_Exists(libraryDir)) {
        if (!FileSystem::BE_Create_Directory(libraryDir)) {
            BE_LOG_ERROR(ShaderImporterLog, "Failed to create Library directory: {}", libraryDir);
            return false;
        }
    }
    
    // Destination path for the imported binary
    const std::string dstPath = FileSystem::BE_Combine_Paths(libraryDir, ctx.guid + ".bin");
    
    // Read the shader source code
    std::string shaderSource;
    try {
        shaderSource = FileSystem::BE_Read_Text_File(fullAssetPath);
    } catch (const std::exception& e) {
        BE_LOG_ERROR(ShaderImporterLog, "Failed to read shader source: {}", e.what());
        return false;
    }
    
    // Encode the shader source as base64
    std::string encodedSource = Base64Encode(shaderSource);
    
    // Write the encoded source to the binary file
    std::ofstream outFile(dstPath, std::ios::binary);
    if (!outFile.is_open()) {
        BE_LOG_ERROR(ShaderImporterLog, "Failed to open output file for writing: {}", dstPath);
        return false;
    }
    
    outFile.write(encodedSource.c_str(), static_cast<std::streamsize>(encodedSource.size()));
    outFile.close();
    
    BE_LOG_INFO(ShaderImporterLog, "Shader imported successfully to {}", dstPath);
    return true;
}

std::string ShaderImporter::Base64Encode(const std::string& input) {
    static const std::string base64Chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string encoded;
    int i = 0;
    unsigned char array3[3];
    unsigned char array4[4];
    
    for (const char c : input) {
        array3[i++] = c;
        if (i == 3) {
            array4[0] = (array3[0] & 0xfc) >> 2;
            array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
            array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
            array4[3] = array3[2] & 0x3f;
            
            for (i = 0; i < 4; i++) {
                encoded += base64Chars[array4[i]];
            }
            i = 0;
        }
    }
    
    if (i) {
        int j = 0;
        for (j = i; j < 3; j++) {
            array3[j] = '\0';
        }
        
        array4[0] = (array3[0] & 0xfc) >> 2;
        array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
        array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
        
        for (j = 0; j < i + 1; j++) {
            encoded += base64Chars[array4[j]];
        }
        
        while (i++ < 3) {
            encoded += '=';
        }
    }
    
    return encoded;
}

} // namespace BlackEngine
