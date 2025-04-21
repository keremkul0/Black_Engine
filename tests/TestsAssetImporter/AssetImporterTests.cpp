#include "gtest/gtest.h"
#include "Core/AssetDatabase/AssetDatabase.h"
#include "Core/AssetImporter/ImporterRegistry.h"
#include "Core/AssetImporter/TextureImporter.h"
#include "Core/AssetImporter/MeshImporter.h"
#include "Core/AssetImporter/ShaderImporter.h"
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Utils/MetaFile.h"
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace BlackEngine;

class AssetImporterTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary project for testing
        const std::string tempDir = fs::temp_directory_path().string() + "/BlackEngineTest_" + 
                                     std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        m_TempProjectPath = tempDir;
        fs::create_directories(m_TempProjectPath);
        fs::create_directories(m_TempProjectPath + "/Assets");
        fs::create_directories(m_TempProjectPath + "/Library");
          // Set this as the active project
        ProjectManager::GetInstance().SetProjectPath(m_TempProjectPath);
        
        // Create test assets
        CreateTestAssets();
    }
    
    void TearDown() override {
        try {
            // Clean up the temporary project
            fs::remove_all(m_TempProjectPath);
        } catch (const std::exception& e) {
            std::cerr << "Error removing temp directory: " << e.what() << std::endl;
        }
    }
    
    void CreateTestAssets() const {
        // Create a test PNG file
        const std::string testPngPath = m_TempProjectPath + "/Assets/test.png";
        CreateDummyBinaryFile(testPngPath, 100);
        
        // Create a test OBJ file with minimal content
        const std::string testObjPath = m_TempProjectPath + "/Assets/test.obj";
        std::ofstream objFile(testObjPath);
        objFile << "v 0.0 0.0 0.0\n";
        objFile << "v 1.0 0.0 0.0\n";
        objFile << "v 1.0 1.0 0.0\n";
        objFile << "f 1 2 3\n";
        objFile.close();
        
        // Create a test shader file
        const std::string testShaderPath = m_TempProjectPath + "/Assets/test.glsl";
        std::ofstream shaderFile(testShaderPath);
        shaderFile << "void main() {\n";
        shaderFile << "    gl_Position = vec4(1.0, 1.0, 1.0, 1.0);\n";
        shaderFile << "}\n";
        shaderFile.close();
    }

    static void CreateDummyBinaryFile(const std::string& path, size_t sizeInBytes) {
        std::ofstream file(path, std::ios::binary);
        std::vector<char> buffer(sizeInBytes, 0);
        file.write(buffer.data(), sizeInBytes);
        file.close();
    }
    
    std::string m_TempProjectPath;
};

TEST_F(AssetImporterTests, TextureImporterTest) {
    // Import the test PNG file
    BlackEngine::AssetDatabase& assetDb = BlackEngine::AssetDatabase::GetInstance();
    
    // Import the asset
    ASSERT_TRUE(assetDb.ImportAsset(m_TempProjectPath + "/Assets/test.png"));
    
    // Get the GUID of the imported asset from its meta file
    nlohmann::json metaData = MetaFile::Load(m_TempProjectPath + "/Assets/test.png");
    ASSERT_FALSE(metaData.empty());
    ASSERT_TRUE(metaData.contains("guid"));
    
    std::string guid = metaData["guid"];
    ASSERT_FALSE(guid.empty());
    
    // Check if the asset is in the database
    const std::string& assetPath = assetDb.GetAssetPath(guid);
    ASSERT_FALSE(assetPath.empty());
    
    // Reimport the asset
    ASSERT_TRUE(assetDb.Reimport(guid));
    
    // Check if the binary file was created
    std::string binaryPath = m_TempProjectPath + "/Library/" + guid + ".bin";
    ASSERT_TRUE(fs::exists(binaryPath));
    
    // Check the size of the binary file (should be the same as the source file)
    ASSERT_EQ(fs::file_size(binaryPath), fs::file_size(m_TempProjectPath + "/Assets/test.png"));
}

TEST_F(AssetImporterTests, MeshImporterTest) {
    // Import the test OBJ file
    BlackEngine::AssetDatabase& assetDb = BlackEngine::AssetDatabase::GetInstance();
    
    // Import the asset
    ASSERT_TRUE(assetDb.ImportAsset(m_TempProjectPath + "/Assets/test.obj"));
    
    // Get the GUID of the imported asset from its meta file
    nlohmann::json metaData = MetaFile::Load(m_TempProjectPath + "/Assets/test.obj");
    ASSERT_FALSE(metaData.empty());
    ASSERT_TRUE(metaData.contains("guid"));
    
    std::string guid = metaData["guid"];
    ASSERT_FALSE(guid.empty());
    
    // Check if the asset is in the database
    const std::string& assetPath = assetDb.GetAssetPath(guid);
    ASSERT_FALSE(assetPath.empty());
    
    // Reimport the asset
    ASSERT_TRUE(assetDb.Reimport(guid));
    
    // Check if the binary file was created
    std::string binaryPath = m_TempProjectPath + "/Library/" + guid + ".bin";
    ASSERT_TRUE(fs::exists(binaryPath));
    
    // Check that the binary file is not empty (contains parsed vertex data)
    ASSERT_GT(fs::file_size(binaryPath), 0);
}

TEST_F(AssetImporterTests, ShaderImporterTest) {
    // Import the test GLSL file
    BlackEngine::AssetDatabase& assetDb = BlackEngine::AssetDatabase::GetInstance();
    
    // Import the asset
    ASSERT_TRUE(assetDb.ImportAsset(m_TempProjectPath + "/Assets/test.glsl"));
    
    // Get the GUID of the imported asset from its meta file
    nlohmann::json metaData = MetaFile::Load(m_TempProjectPath + "/Assets/test.glsl");
    ASSERT_FALSE(metaData.empty());
    ASSERT_TRUE(metaData.contains("guid"));
    
    std::string guid = metaData["guid"];
    ASSERT_FALSE(guid.empty());
    
    // Check if the asset is in the database
    const std::string& assetPath = assetDb.GetAssetPath(guid);
    ASSERT_FALSE(assetPath.empty());
    
    // Reimport the asset
    ASSERT_TRUE(assetDb.Reimport(guid));
    
    // Check if the binary file was created
    std::string binaryPath = m_TempProjectPath + "/Library/" + guid + ".bin";
    ASSERT_TRUE(fs::exists(binaryPath));
    
    // Check that the binary file is not empty (contains base64 encoded shader source)
    ASSERT_GT(fs::file_size(binaryPath), 0);
}

TEST_F(AssetImporterTests, ImporterRegistryTest) {
    // Test that the importer registry returns the correct importers for different extensions
    BlackEngine::ImporterRegistry& registry = BlackEngine::ImporterRegistry::GetInstance();
    
    // Test PNG extension
    BlackEngine::IAssetImporter* textureImporter = registry.GetImporterForExtension(".png");
    ASSERT_NE(textureImporter, nullptr);
      // Test OBJ extension
    BlackEngine::IAssetImporter* meshImporter = registry.GetImporterForExtension(".obj");
    ASSERT_NE(meshImporter, nullptr);
    
    // Test GLSL extension
    BlackEngine::IAssetImporter* shaderImporter = registry.GetImporterForExtension(".glsl");
    ASSERT_NE(shaderImporter, nullptr);
    
    // Test unsupported extension
    BlackEngine::IAssetImporter* unknownImporter = registry.GetImporterForExtension(".xyz");
    ASSERT_EQ(unknownImporter, nullptr);
}

TEST_F(AssetImporterTests, AssetDatabaseReimportTest) {
    BlackEngine::AssetDatabase& assetDb = BlackEngine::AssetDatabase::GetInstance();
    
    // Import all test assets
    ASSERT_TRUE(assetDb.ImportAsset(m_TempProjectPath + "/Assets/test.png"));
    ASSERT_TRUE(assetDb.ImportAsset(m_TempProjectPath + "/Assets/test.obj"));
    ASSERT_TRUE(assetDb.ImportAsset(m_TempProjectPath + "/Assets/test.glsl"));
    
    // Get the GUIDs of the imported assets
    nlohmann::json pngMeta = MetaFile::Load(m_TempProjectPath + "/Assets/test.png");
    nlohmann::json objMeta = MetaFile::Load(m_TempProjectPath + "/Assets/test.obj");
    nlohmann::json glslMeta = MetaFile::Load(m_TempProjectPath + "/Assets/test.glsl");
    
    std::string pngGuid = pngMeta["guid"];
    std::string objGuid = objMeta["guid"];
    std::string glslGuid = glslMeta["guid"];
    
    // Test reimport of all assets
    ASSERT_TRUE(assetDb.Reimport(pngGuid));
    ASSERT_TRUE(assetDb.Reimport(objGuid));
    ASSERT_TRUE(assetDb.Reimport(glslGuid));
    
    // Check if all binary files exist
    ASSERT_TRUE(fs::exists(m_TempProjectPath + "/Library/" + pngGuid + ".bin"));
    ASSERT_TRUE(fs::exists(m_TempProjectPath + "/Library/" + objGuid + ".bin"));
    ASSERT_TRUE(fs::exists(m_TempProjectPath + "/Library/" + glslGuid + ".bin"));
    
    // Try reimporting a non-existent GUID
    ASSERT_FALSE(assetDb.Reimport("non-existent-guid"));
}
