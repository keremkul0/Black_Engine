#include <gtest/gtest.h>
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/Utils/GuidUtils.h"
#include "Core/Utils/MetaFile.h"
#include "Core/FileSystem/FileSystem.h"
#include <string>
#include <set>
#include <filesystem>

namespace BlackEngine::Tests {
    class ProjectManagerTests : public ::testing::Test {
    protected:
        void SetUp() override {
            // Create a temporary test directory
            m_TestDir = std::filesystem::temp_directory_path().string() + "/BlackEngineTest";

            // Clean up if it exists from previous tests
            if (std::filesystem::exists(m_TestDir)) {
                std::filesystem::remove_all(m_TestDir);
            }

            // Create the test directory
            ASSERT_TRUE(FileSystem::BE_Create_Directory(m_TestDir));
        }

        void TearDown() override {
            // Clean up after tests
            if (std::filesystem::exists(m_TestDir)) {
                std::filesystem::remove_all(m_TestDir);
            }
        }

        std::string m_TestDir;
    };

    // Test that CreateNewProject creates the directory structure correctly
    TEST_F(ProjectManagerTests, CreateNewProject_CreatesDirectoryStructure) {
        // Arrange
        std::string projectPath = m_TestDir + "/TestProject";
        auto &projectManager = ProjectManager::GetInstance();

        // Act
        bool result = projectManager.CreateNewProject(projectPath);

        // Assert
        EXPECT_TRUE(result);

        // Check that all required directories were created
        EXPECT_TRUE(std::filesystem::exists(projectPath));
        EXPECT_TRUE(std::filesystem::exists(projectPath + "/Assets"));
        EXPECT_TRUE(std::filesystem::exists(projectPath + "/ProjectSettings"));
        EXPECT_TRUE(std::filesystem::exists(projectPath + "/Library"));

        // Verify project.json exists and has correct structure
        std::string projectJsonPath = projectPath + "/project.json";
        EXPECT_TRUE(std::filesystem::exists(projectJsonPath));

        // Read and verify project.json content
        std::string jsonContent = FileSystem::BE_Read_Text_File(projectJsonPath);
        auto projectData = nlohmann::json::parse(jsonContent);

        EXPECT_EQ("TestProject", projectData["name"]);
        EXPECT_EQ("1.0.0", projectData["version"]);
        EXPECT_EQ("Black Engine", projectData["engine"]);

        // Verify settings.json exists and has correct structure
        std::string settingsJsonPath = projectPath + "/ProjectSettings/settings.json";
        EXPECT_TRUE(std::filesystem::exists(settingsJsonPath));

        // Read and verify settings.json content
        jsonContent = FileSystem::BE_Read_Text_File(settingsJsonPath);
        auto settingsData = nlohmann::json::parse(jsonContent);

        EXPECT_EQ("Build", settingsData["build"]["outputDirectory"]);
        EXPECT_EQ("Debug", settingsData["build"]["buildType"]);
        EXPECT_EQ(300, settingsData["editor"]["autoSaveInterval"]);
        EXPECT_EQ("Dark", settingsData["editor"]["theme"]);
        EXPECT_TRUE(settingsData["editor"]["showGrid"]);
        EXPECT_EQ(4, settingsData["render"]["msaaLevel"]);
        EXPECT_EQ("Medium", settingsData["render"]["shadowQuality"]);
        EXPECT_TRUE(settingsData["render"]["useHDR"]);
    }

    TEST_F(ProjectManagerTests, CreateNewProject_ReturnsFalseIfPathAlreadyExists) {
        // Arrange
        const std::string projectPath = m_TestDir + "/AlreadyExistsTest";
        auto &projectManager = ProjectManager::GetInstance();

        // Act - First call (should succeed)
        const bool firstResult = projectManager.CreateNewProject(projectPath);

        // Assert - First call successful
        EXPECT_TRUE(firstResult);
        EXPECT_TRUE(std::filesystem::exists(projectPath));

        // Act - Second call to the same path (should fail)
        const bool secondResult = projectManager.CreateNewProject(projectPath);

        // Assert - Second call should fail because path already exists
        EXPECT_FALSE(secondResult);
    }


    // Test LoadProject with missing project.json
    TEST_F(ProjectManagerTests, LoadProject_MissingProjectJson) {
        // Arrange
        const std::string projectPath = m_TestDir + "/MissingProject";
        auto &projectManager = ProjectManager::GetInstance();

        // Create directory without project.json
        ASSERT_TRUE(FileSystem::BE_Create_Directory(projectPath));

        // Act
        const bool result = projectManager.LoadProject(projectPath);

        // Assert
        EXPECT_FALSE(result);
    }

    // Test LoadProject with malformed project.json
    TEST_F(ProjectManagerTests, LoadProject_MalformedProjectJson) {
        // Arrange
        std::string projectPath = m_TestDir + "/MalformedProject";
        auto &projectManager = ProjectManager::GetInstance();

        // Create directory
        ASSERT_TRUE(FileSystem::BE_Create_Directory(projectPath));

        // Create malformed project.json
        std::string projectJsonPath = projectPath + "/project.json";
        ASSERT_TRUE(FileSystem::BE_Write_Text_File(projectJsonPath, "{ this is not valid JSON }"));

        // Act
        bool result = projectManager.LoadProject(projectPath);

        // Assert
        EXPECT_FALSE(result);

        // Check that default settings are used when loading fails
        ProjectSettings defaultSettings;
        const ProjectSettings &currentSettings = projectManager.GetSettings();

        EXPECT_EQ(defaultSettings.buildSettings.outputDirectory, currentSettings.buildSettings.outputDirectory);
        EXPECT_EQ(defaultSettings.editorSettings.autoSaveInterval, currentSettings.editorSettings.autoSaveInterval);
        EXPECT_EQ(defaultSettings.renderSettings.msaaLevel, currentSettings.renderSettings.msaaLevel);
    }

    // Test CreateDefaultSettings creates settings.json when missing
    TEST_F(ProjectManagerTests, CreateDefaultSettings_CreatesSettingsJson) {
        // Arrange
        std::string projectPath = m_TestDir + "/DefaultSettingsTest";
        auto &projectManager = ProjectManager::GetInstance();

        // Create a project without initial settings
        ASSERT_TRUE(projectManager.CreateNewProject(projectPath));

        // Delete the settings file to simulate missing settings
        std::string settingsPath = projectPath + "/ProjectSettings/settings.json";
        std::filesystem::remove(settingsPath);
        ASSERT_FALSE(std::filesystem::exists(settingsPath));

        // Act - Load the project, which should trigger CreateDefaultSettings
        ASSERT_TRUE(projectManager.LoadProject(projectPath));

        // Assert - Check that settings.json was created
        EXPECT_TRUE(std::filesystem::exists(settingsPath));

        // Verify content of settings.json
        std::string jsonContent = FileSystem::BE_Read_Text_File(settingsPath);
        auto settingsData = nlohmann::json::parse(jsonContent);

        EXPECT_EQ("Build", settingsData["build"]["outputDirectory"]);
        EXPECT_EQ("Debug", settingsData["build"]["buildType"]);
        EXPECT_EQ(300, settingsData["editor"]["autoSaveInterval"]);
    }

    // Test SaveProjectSettings properly saves changes
    TEST_F(ProjectManagerTests, SaveProjectSettings_SavesChanges) {
        // Arrange
        std::string projectPath = m_TestDir + "/SaveSettingsTest";
        auto &projectManager = ProjectManager::GetInstance();

        // Create a project with initial settings
        ASSERT_TRUE(projectManager.CreateNewProject(projectPath));

        // Act - Modify settings and save
        ProjectSettings &settings = projectManager.GetMutableSettings();
        settings.buildSettings.outputDirectory = "CustomBuildDir";
        settings.editorSettings.theme = "Light";
        settings.renderSettings.msaaLevel = 8;

        // Save the modified settings
        EXPECT_TRUE(projectManager.SaveProjectSettings());

        // Assert - Verify the changes were written to disk
        std::string settingsPath = projectPath + "/ProjectSettings/settings.json";
        ASSERT_TRUE(std::filesystem::exists(settingsPath));

        // Read and verify content
        std::string jsonContent = FileSystem::BE_Read_Text_File(settingsPath);
        auto settingsData = nlohmann::json::parse(jsonContent);

        EXPECT_EQ("CustomBuildDir", settingsData["build"]["outputDirectory"]);
        EXPECT_EQ("Light", settingsData["editor"]["theme"]);
        EXPECT_EQ(8, settingsData["render"]["msaaLevel"]);

        // Further test - load the settings again to verify round-trip
        auto &newProjectManager = ProjectManager::GetInstance();
        ASSERT_TRUE(newProjectManager.LoadProject(projectPath));

        const ProjectSettings &loadedSettings = newProjectManager.GetSettings();
        EXPECT_EQ("CustomBuildDir", loadedSettings.buildSettings.outputDirectory);
        EXPECT_EQ("Light", loadedSettings.editorSettings.theme);
        EXPECT_EQ(8, loadedSettings.renderSettings.msaaLevel);
    }

    // Test SaveProject saves both project.json and settings.json
    TEST_F(ProjectManagerTests, SaveProject_SavesAllFiles) {
        // Arrange
        std::string projectPath = m_TestDir + "/SaveProjectTest";
        auto &projectManager = ProjectManager::GetInstance();

        // Create a project
        ASSERT_TRUE(projectManager.CreateNewProject(projectPath));

        // Act - Modify project details and settings
        ProjectSettings &settings = projectManager.GetMutableSettings();
        settings.renderSettings.shadowQuality = "High";

        // Save the project with modified settings
        EXPECT_TRUE(projectManager.SaveProject());

        // Assert - Verify changes in both files
        std::string settingsPath = projectPath + "/ProjectSettings/settings.json";
        std::string projectJsonPath = projectPath + "/project.json";

        ASSERT_TRUE(std::filesystem::exists(settingsPath));
        ASSERT_TRUE(std::filesystem::exists(projectJsonPath));

        // Verify settings.json
        std::string settingsContent = FileSystem::BE_Read_Text_File(settingsPath);
        auto settingsData = nlohmann::json::parse(settingsContent);
        EXPECT_EQ("High", settingsData["render"]["shadowQuality"]);

        // Verify project.json
        std::string projectContent = FileSystem::BE_Read_Text_File(projectJsonPath);
        auto projectData = nlohmann::json::parse(projectContent);
        EXPECT_EQ("SaveProjectTest", projectData["name"]);
        EXPECT_EQ("1.0.0", projectData["version"]);
        EXPECT_EQ("Black Engine", projectData["engine"]);
    }

    // Test that GenerateGuid creates unique GUIDs
    TEST_F(ProjectManagerTests, GenerateGuid_CreatesUniqueGuids) {
        // Arrange
        constexpr int guidCount = 100000;
        std::set<std::string> guids;

        // Act
        for (int i = 0; i < guidCount; i++) {
            std::string guid = GuidUtils::GenerateGuid();
            guids.insert(guid);

            // Check RFC-4122 v4 GUID format
            ASSERT_EQ(36, guid.length());
            ASSERT_EQ('-', guid[8]);
            ASSERT_EQ('-', guid[13]);
            ASSERT_EQ('-', guid[18]);
            ASSERT_EQ('-', guid[23]);
            ASSERT_EQ('4', guid[14]); // Version 4

            // Check that the 17th character is 8, 9, A, or B (variant)
            char variantChar = guid[19];
            ASSERT_TRUE(variantChar == '8' || variantChar == '9' ||
                variantChar == 'a' || variantChar == 'b' ||
                variantChar == 'A' || variantChar == 'B');
        }

        // Assert - all GUIDs should be unique
        EXPECT_EQ(guidCount, guids.size());
    }

    // Test that MetaFile creates and saves meta files correctly
    TEST_F(ProjectManagerTests, MetaFile_CreatesAndSavesMetaFiles) {
        // Arrange
        std::string assetPath = m_TestDir + "/TestAsset.txt";

        // Create a test asset
        ASSERT_TRUE(FileSystem::BE_Write_Text_File(assetPath, "Test asset content"));

        // Act
        bool result = MetaFile::Create(assetPath, "TextAsset", "DefaultImporter");

        // Assert
        EXPECT_TRUE(result);

        // Check that the meta file was created
        std::string metaFilePath = assetPath + ".meta";
        EXPECT_TRUE(std::filesystem::exists(metaFilePath));

        // Load the meta file and check its content
        json metaData = MetaFile::Load(assetPath);
        EXPECT_FALSE(metaData.empty());
        EXPECT_TRUE(metaData.contains("guid"));
        EXPECT_EQ("TextAsset", metaData["type"]);
        EXPECT_EQ("DefaultImporter", metaData["importer"]);
        EXPECT_TRUE(metaData.contains("settings"));
    }
} // namespace BlackEngine::Tests
