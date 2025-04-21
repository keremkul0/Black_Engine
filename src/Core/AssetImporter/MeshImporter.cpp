#include "MeshImporter.h"
#include "Core/Logger/LogMacros.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/ProjectManager/ProjectManager.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

BE_DEFINE_LOG_CATEGORY(MeshImporterLog, "MeshImporter");

namespace BlackEngine {

std::vector<std::string> MeshImporter::SupportedExtensions() const {
    return {".obj"};
}

bool MeshImporter::Import(const ImportContext& ctx) {
    BE_LOG_INFO(MeshImporterLog, "Importing mesh: {}", ctx.assetPath);
    
    if (ctx.guid.empty()) {
        BE_LOG_ERROR(MeshImporterLog, "Cannot import mesh with empty GUID");
        return false;
    }
    
    // Get the project path
    const std::string projectPath = ProjectManager::GetInstance().GetProjectPath();
    if (projectPath.empty()) {
        BE_LOG_ERROR(MeshImporterLog, "Cannot import mesh: No active project");
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
        BE_LOG_ERROR(MeshImporterLog, "Source file does not exist: {}", fullAssetPath);
        return false;
    }
    
    // Create the Library directory if it doesn't exist
    const std::string libraryDir = FileSystem::BE_Combine_Paths(projectPath, "Library");
    if (!FileSystem::BE_Directory_Exists(libraryDir)) {
        if (!FileSystem::BE_Create_Directory(libraryDir)) {
            BE_LOG_ERROR(MeshImporterLog, "Failed to create Library directory: {}", libraryDir);
            return false;
        }
    }
    
    // Destination path for the imported binary
    const std::string dstPath = FileSystem::BE_Combine_Paths(libraryDir, ctx.guid + ".bin");
    
    // Get file extension to determine the proper importer function
    std::string extension = FileSystem::BE_Get_File_Extension(fullAssetPath);
    std::ranges::transform(extension, extension.begin(), ::tolower);
    
    // Dispatch to the correct importer based on file extension
    if (extension == ".obj") {
        return ImportOBJ(fullAssetPath, dstPath);
    }
    
    // Unsupported mesh format
    BE_LOG_ERROR(MeshImporterLog, "Unsupported mesh format: {}", extension);
    return false;
}

bool MeshImporter::ImportOBJ(const std::string& objFilePath, const std::string& outputPath) {
    BE_LOG_INFO(MeshImporterLog, "Parsing OBJ file: {}", objFilePath);
    
    std::ifstream file(objFilePath);
    if (!file.is_open()) {
        BE_LOG_ERROR(MeshImporterLog, "Failed to open OBJ file: {}", objFilePath);
        return false;
    }
    
    // Temporary vectors to store the data
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> finalVertices; // This will be the flattened array we write to the binary file
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            // Vertex position
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if (prefix == "vn") {
            // Vertex normal
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        } else if (prefix == "vt") {
            // Texture coordinate
            float u, v;
            iss >> u >> v;
            uvs.push_back(u);
            uvs.push_back(v);
        } else if (prefix == "f") {
            // Face definition
            // Format: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            // For simplicity, we'll just extract the vertex indices and create a triangle
            std::string vertexDef;
            std::vector<int> vertexIndices;
            std::vector<int> uvIndices;
            std::vector<int> normalIndices;
            
            while (iss >> vertexDef) {
                std::ranges::replace(vertexDef, '/', ' ');
                std::istringstream vdef(vertexDef);
                int vIndex;
                vdef >> vIndex;
                vertexIndices.push_back(vIndex);
                
                if (int uvIndex = -1; vdef >> uvIndex) {
                    uvIndices.push_back(uvIndex);
                }
                
                if (int nIndex = -1; vdef >> nIndex) {
                    normalIndices.push_back(nIndex);
                }
            }
            
            // Create a triangle (OBJ indices are 1-based, so subtract 1)
            for (size_t i = 0; i < vertexIndices.size(); ++i) {
                int vIndex = vertexIndices[i] - 1;
                
                // Add vertex position (x, y, z)
                finalVertices.push_back(vertices[vIndex * 3]);
                finalVertices.push_back(vertices[vIndex * 3 + 1]);
                finalVertices.push_back(vertices[vIndex * 3 + 2]);
                
                // Add UV coordinates if available (u, v)
                if (i < uvIndices.size() && uvIndices[i] > 0) {
                    int uvIndex = uvIndices[i] - 1;
                    finalVertices.push_back(uvs[uvIndex * 2]);
                    finalVertices.push_back(uvs[uvIndex * 2 + 1]);
                } else {
                    // Default UV coordinates
                    finalVertices.push_back(0.0f);
                    finalVertices.push_back(0.0f);
                }
                
                // Add normals if available (nx, ny, nz)
                if (i < normalIndices.size() && normalIndices[i] > 0) {
                    int nIndex = normalIndices[i] - 1;
                    finalVertices.push_back(normals[nIndex * 3]);
                    finalVertices.push_back(normals[nIndex * 3 + 1]);
                    finalVertices.push_back(normals[nIndex * 3 + 2]);
                } else {
                    // Default normal
                    finalVertices.push_back(0.0f);
                    finalVertices.push_back(1.0f);
                    finalVertices.push_back(0.0f);
                }
            }
        }
    }
    
    file.close();
    
    // Write the flattened vertex data to a binary file
    if (finalVertices.empty()) {
        BE_LOG_ERROR(MeshImporterLog, "No vertex data found in OBJ file");
        return false;
    }
    
    BE_LOG_INFO(MeshImporterLog, "Extracted {} vertices from OBJ file", finalVertices.size() / 8);
    return WriteBinaryData(outputPath, finalVertices);
}

bool MeshImporter::WriteBinaryData(const std::string& filePath, const std::vector<float>& data) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        BE_LOG_ERROR(MeshImporterLog, "Failed to open output file for writing: {}", filePath);
        return false;
    }
    
    // Write the number of vertices as a header
    uint32_t vertexCount = static_cast<uint32_t>(data.size() / 8); // 8 = 3 pos + 2 uv + 3 normal
    outFile.write(reinterpret_cast<char*>(&vertexCount), sizeof(uint32_t));
    
    // Write the vertex data
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
    
    outFile.close();
    
    BE_LOG_INFO(MeshImporterLog, "Wrote {} bytes to {}", data.size() * sizeof(float) + sizeof(uint32_t), filePath);
    return true;
}

} // namespace BlackEngine
