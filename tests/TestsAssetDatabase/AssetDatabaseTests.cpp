#include <gtest/gtest.h>
#include "Core/AssetDatabase/AssetDatabase.h"
#include "Core/AssetImporter/AssetImporterRegistry.h"
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Utils/MetaFile.h"
#include <filesystem>
#include <string>

namespace BlackEngine::Tests {
    class AssetDatabaseTests : public ::testing::Test {
    protected:        void SetUp() override {
            // Create a temporary test directory
            m_TestDir = std::filesystem::temp_directory_path().string() + "/BlackEngineAssetTest";

            // Clean up if it exists from previous tests
            if (std::filesystem::exists(m_TestDir)) {
                std::filesystem::remove_all(m_TestDir);
            }

            // Create the test directory
            ASSERT_TRUE(FileSystem::BE_Create_Directory(m_TestDir));

            // Create a test project to work with
            std::string projectPath = m_TestDir + "/TestProject";
            auto &projectManager = ProjectManager::GetInstance();
            ASSERT_TRUE(projectManager.CreateNewProject(projectPath));

            // Initialize asset importers to ensure all importers are registered
            BlackEngine::InitializeAssetImporters();

            // Create some test assets
            m_TestAssetDir = m_TestDir + "/TestAssets";
            ASSERT_TRUE(FileSystem::BE_Create_Directory(m_TestAssetDir));

            // Create a test texture file
            m_TestTexturePath = m_TestAssetDir + "/test_texture.png";
            ASSERT_TRUE(FileSystem::BE_Write_Text_File(m_TestTexturePath, "Fake PNG content"));

            // (Removed .cs test script creation)
        }

        void TearDown() override {
            // Clean up after tests
            if (std::filesystem::exists(m_TestDir)) {
                std::filesystem::remove_all(m_TestDir);
            }
        }

        std::string m_TestDir;
        std::string m_TestAssetDir;
        std::string m_TestTexturePath;
        std::string m_TestScriptPath;
    };

    // Test that ImportAsset imports a file into the Assets directory and creates a .meta file
    TEST_F(AssetDatabaseTests, ImportAsset_ImportsFileAndCreatesMeta) {
        // Arrange
        auto &assetDatabase = AssetDatabase::GetInstance();
        auto &projectManager = ProjectManager::GetInstance();
        std::string projectPath = projectManager.GetProjectPath();
        std::string assetsPath = projectPath + "/Assets";

        // Act
        bool result = assetDatabase.ImportAsset(m_TestTexturePath);

        // Assert
        EXPECT_TRUE(result);

        // Check that the file was copied to the Assets directory
        std::string destPath = assetsPath + "/test_texture.png";
        EXPECT_TRUE(std::filesystem::exists(destPath));

        // Check that a meta file was created
        std::string metaPath = destPath + ".meta";
        EXPECT_TRUE(std::filesystem::exists(metaPath));

        // Load the meta file and verify its content
        json metaData = MetaFile::Load(destPath);
        EXPECT_FALSE(metaData.empty());
        EXPECT_TRUE(metaData.contains("guid"));
        EXPECT_EQ("Texture", metaData["type"]);
        EXPECT_EQ("TextureImporter", metaData["importer"]);
    }

    // Test that ImportAsset preserves GUID when reimporting
    TEST_F(AssetDatabaseTests, ImportAsset_PreservesGuid) {
        // Arrange
        auto &assetDatabase = AssetDatabase::GetInstance();
        auto &projectManager = ProjectManager::GetInstance();
        std::string projectPath = projectManager.GetProjectPath();
        std::string assetsPath = projectPath + "/Assets";

        // Import the file first time
        ASSERT_TRUE(assetDatabase.ImportAsset(m_TestTexturePath));

        // Get the GUID from the meta file
        std::string destPath = assetsPath + "/test_texture.png";
        json metaData = MetaFile::Load(destPath);
        std::string originalGuid = metaData["guid"];

        // Modify the source file
        ASSERT_TRUE(FileSystem::BE_Write_Text_File(m_TestTexturePath, "Modified PNG content"));

        // Act - Import the file second time
        bool result = assetDatabase.ImportAsset(m_TestTexturePath);

        // Assert
        EXPECT_TRUE(result);

        // Check that the GUID remains the same
        json updatedMetaData = MetaFile::Load(destPath);
        std::string newGuid = updatedMetaData["guid"];

        EXPECT_EQ(originalGuid, newGuid);
    }

    // Test that asset_map.json is created and populated correctly
    TEST_F(AssetDatabaseTests, RefreshAssetCache_CreatesAssetMapJson) {
        // Arrange
        auto &assetDatabase = AssetDatabase::GetInstance();
        auto &projectManager = ProjectManager::GetInstance();
        std::string projectPath = projectManager.GetProjectPath();
        std::string assetsPath = projectPath + "/Assets";
        std::string libraryPath = projectPath + "/Library";

        // Import multiple assets
        ASSERT_TRUE(assetDatabase.ImportAsset(m_TestTexturePath));
        // Import a .lua script instead
        std::string testLuaPath = m_TestAssetDir + "/test_script.lua";
        ASSERT_TRUE(FileSystem::BE_Write_Text_File(testLuaPath, "print('Hello from Lua!')"));
        ASSERT_TRUE(assetDatabase.ImportAsset(testLuaPath));

        // Act
        assetDatabase.RefreshAssetCache();

        // Assert
        // Check that asset_map.json was created
        std::string assetMapPath = libraryPath + "/asset_map.json";
        EXPECT_TRUE(std::filesystem::exists(assetMapPath));

        // Load asset_map.json and verify its content
        json assetMap = FileSystem::BE_Read_JSON(assetMapPath);
        EXPECT_FALSE(assetMap.empty());

        // Get the GUIDs from the meta files
        json textureMetaData = MetaFile::Load(assetsPath + "/test_texture.png");
        json luaMetaData = MetaFile::Load(assetsPath + "/test_script.lua");

        std::string textureGuid = textureMetaData["guid"];
        std::string luaGuid = luaMetaData["guid"];

        // Verify that the asset map contains entries for both assets
        EXPECT_TRUE(assetMap.contains(textureGuid));
        EXPECT_TRUE(assetMap.contains(luaGuid));

        // Verify that the paths are correct
        EXPECT_EQ("Assets/test_texture.png", assetMap[textureGuid]);
        EXPECT_EQ("Assets/test_script.lua", assetMap[luaGuid]);
    }

    // Test that GetAssetPath returns the correct path for a GUID
    TEST_F(AssetDatabaseTests, GetAssetPath_ReturnsCorrectPath) {
        // Arrange
        auto &assetDatabase = AssetDatabase::GetInstance();
        auto &projectManager = ProjectManager::GetInstance();
        std::string projectPath = projectManager.GetProjectPath();
        std::string assetsPath = projectPath + "/Assets";

        // Import an asset
        ASSERT_TRUE(assetDatabase.ImportAsset(m_TestTexturePath));

        // Get the GUID from the meta file
        std::string destPath = assetsPath + "/test_texture.png";
        json metaData = MetaFile::Load(destPath);
        std::string guid = metaData["guid"];

        // Make sure the cache is refreshed
        assetDatabase.RefreshAssetCache();

        // Act
        const std::string &assetPath = assetDatabase.GetAssetPath(guid);

        // Assert
        EXPECT_EQ("Assets/test_texture.png", assetPath);
    }

    // Test that GetAssetPath returns empty string for an unknown GUID
    TEST_F(AssetDatabaseTests, GetAssetPath_ReturnsEmptyForUnknownGuid) {
        // Arrange
        auto &assetDatabase = AssetDatabase::GetInstance();

        // Make sure the cache is refreshed
        assetDatabase.RefreshAssetCache();

        // Act
        const std::string &assetPath = assetDatabase.GetAssetPath("non-existent-guid");

        // Assert
        EXPECT_TRUE(assetPath.empty());
    }

    // Test that meta files are created for assets that don't have them
    TEST_F(AssetDatabaseTests, RefreshAssetCache_CreatesMissingMetaFiles) {
        // Arrange
        auto &assetDatabase = AssetDatabase::GetInstance();
        auto &projectManager = ProjectManager::GetInstance();
        std::string projectPath = projectManager.GetProjectPath();
        std::string assetsPath = projectPath + "/Assets";

        // Create a file directly in the Assets directory without a meta file
        std::string directAssetPath = assetsPath + "/direct_asset.txt";
        ASSERT_TRUE(FileSystem::BE_Write_Text_File(directAssetPath, "Direct asset content"));

        // Act
        assetDatabase.RefreshAssetCache();

        // Assert
        // Check that a meta file was created
        std::string metaPath = directAssetPath + ".meta";
        EXPECT_TRUE(std::filesystem::exists(metaPath));

        // Load the meta file and verify its content
        json metaData = MetaFile::Load(directAssetPath);
        EXPECT_FALSE(metaData.empty());
        EXPECT_TRUE(metaData.contains("guid"));
        EXPECT_EQ("TextAsset", metaData["type"]);
        EXPECT_EQ("TextAssetImporter", metaData["importer"]);

        // Verify that the asset is in the asset map
        std::string libraryPath = projectPath + "/Library";
        std::string assetMapPath = libraryPath + "/asset_map.json";
        json assetMap = FileSystem::BE_Read_JSON(assetMapPath);

        std::string guid = metaData["guid"];
        EXPECT_TRUE(assetMap.contains(guid));
        EXPECT_EQ("Assets/direct_asset.txt", assetMap[guid]);
    }
} // namespace BlackEngine::Tests
