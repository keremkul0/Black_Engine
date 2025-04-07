#include "ProjectManager.h"
#include <string>
#include "Core/FileSystem/FileSystem.h"
#include "Core/SceneManager/SceneManager.h"
#include "Core/Logger/LogMacros.h"

/**
 * Returns the singleton instance of the Project Manager.
 * @return The singleton instance
 */
ProjectManager &ProjectManager::GetInstance() {
    static ProjectManager instance;
    return instance;
}

/**
 * Loads a project from the specified path.
 * @param projectPath Path to the project directory
 * @return True if the project was loaded successfully
 */
bool ProjectManager::LoadProject(const std::string &projectPath) {
    BE_INFO(fmt::format("Loading project from: {}", projectPath));

    // Check if project file exists
    std::string projectFilePath = projectPath + "/project.json";
    if (!FileSystem::FileExists(projectFilePath)) {
        BE_ERROR(fmt::format("Project file not found: {}", projectFilePath));
        return false;
    }

    // Read and parse project file
    BE_DEBUG(fmt::format("Reading project file: {}", projectFilePath));
    std::string projectJson = FileSystem::ReadTextFile(projectFilePath);

    try {
        auto projectData = nlohmann::json::parse(projectJson);

        // Store project information
        m_ProjectName = projectData["name"];
        m_ProjectPath = projectPath;
        m_ProjectVersion = projectData["version"];

        BE_INFO(fmt::format("Project loaded: {}, Version: {}",
            m_ProjectName, m_ProjectVersion));

        // Load project settings
        LoadProjectSettings();

        return true;
    } catch (const std::exception &e) {
        BE_ERROR(fmt::format("Error parsing project file: {}", e.what()));
        return false;
    }
}

/**
 * Returns the current project path.
 * @return The filesystem path to the project
 */
std::string ProjectManager::GetProjectPath() const {
    return m_ProjectPath;
}

/**
 * Loads project settings from settings.json file.
 * Creates default settings if file doesn't exist.
 * @return True if settings were loaded successfully
 */
bool ProjectManager::LoadProjectSettings() {
    BE_DEBUG("Loading project settings");
    std::string settingsFilePath = m_ProjectPath + "/settings.json";

    // Check if settings file exists
    if (!FileSystem::FileExists(settingsFilePath)) {
        BE_INFO(fmt::format("Settings file not found: {}. Creating defaults.",
            settingsFilePath));
        // Create default settings if not found
        CreateDefaultSettings();
        return true;
    }

    // Try to read and parse settings
    try {
        BE_DEBUG(fmt::format("Reading settings file: {}", settingsFilePath));
        std::string settingsJson = FileSystem::ReadTextFile(settingsFilePath);
        auto settingsData = nlohmann::json::parse(settingsJson);

        // Parse build settings
        BE_DEBUG("Parsing build settings");
        auto buildSettings = settingsData["build"];
        m_Settings.buildSettings.outputDirectory = buildSettings["outputDirectory"];
        m_Settings.buildSettings.buildType = buildSettings["buildType"];

        // Parse editor settings
        BE_DEBUG("Parsing editor settings");
        auto editorSettings = settingsData["editor"];
        m_Settings.editorSettings.autoSaveInterval = editorSettings["autoSaveInterval"];
        m_Settings.editorSettings.theme = editorSettings["theme"];
        m_Settings.editorSettings.showGrid = editorSettings["showGrid"];

        // Parse render settings
        BE_DEBUG("Parsing render settings");
        auto renderSettings = settingsData["render"];
        m_Settings.renderSettings.msaaLevel = renderSettings["msaaLevel"];
        m_Settings.renderSettings.shadowQuality = renderSettings["shadowQuality"];
        m_Settings.renderSettings.useHDR = renderSettings["useHDR"];

        BE_INFO("Project settings loaded successfully");
        return true;
    } catch (const std::exception &e) {
        BE_ERROR(fmt::format("Error loading settings: {}. Using defaults.", e.what()));
        // Fall back to default settings on error
        CreateDefaultSettings();
        return false;
    }
}

/**
 * Creates default project settings and saves them to disk.
 */
void ProjectManager::CreateDefaultSettings() {
    BE_INFO("Creating default project settings");

    // Initialize with default values
    m_Settings.buildSettings.outputDirectory = "Build";
    m_Settings.buildSettings.buildType = "Debug";

    m_Settings.editorSettings.autoSaveInterval = 300; // 5 minutes in seconds
    m_Settings.editorSettings.theme = "Dark";
    m_Settings.editorSettings.showGrid = true;

    m_Settings.renderSettings.msaaLevel = 4;
    m_Settings.renderSettings.shadowQuality = "Medium";
    m_Settings.renderSettings.useHDR = true;

    // Save the default settings to disk
    BE_DEBUG("Saving default settings to disk");
    SaveProjectSettings();
}

/**
 * Saves current project settings to settings.json file.
 * @return True if settings were saved successfully
 */
bool ProjectManager::SaveProjectSettings() {
    BE_DEBUG("Saving project settings");

    try {
        // Create JSON structure
        nlohmann::json settingsJson;

        // Build settings
        settingsJson["build"]["outputDirectory"] = m_Settings.buildSettings.outputDirectory;
        settingsJson["build"]["buildType"] = m_Settings.buildSettings.buildType;

        // Editor settings
        settingsJson["editor"]["autoSaveInterval"] = m_Settings.editorSettings.autoSaveInterval;
        settingsJson["editor"]["theme"] = m_Settings.editorSettings.theme;
        settingsJson["editor"]["showGrid"] = m_Settings.editorSettings.showGrid;

        // Render settings
        settingsJson["render"]["msaaLevel"] = m_Settings.renderSettings.msaaLevel;
        settingsJson["render"]["shadowQuality"] = m_Settings.renderSettings.shadowQuality;
        settingsJson["render"]["useHDR"] = m_Settings.renderSettings.useHDR;

        // Save to file
        std::string settingsFilePath = m_ProjectPath + "/settings.json";
        BE_DEBUG(fmt::format("Writing settings file: {}", settingsFilePath));
        bool success = FileSystem::WriteTextFile(settingsFilePath, settingsJson.dump(4));

        if (success) {
            BE_INFO("Project settings saved successfully");
        } else {
            BE_ERROR(fmt::format("Failed to write settings file: {}", settingsFilePath));
        }

        return success;
    } catch (const std::exception &e) {
        BE_ERROR(fmt::format("Error saving settings: {}", e.what()));
        return false;
    }
}

/**
 * Saves the current project. This updates both project.json and
 * settings.json files.
 * @return True if project was saved successfully
 */
bool ProjectManager::SaveProject() {
    BE_INFO(fmt::format("Saving project: {}", m_ProjectName));

    // Save project settings
    BE_DEBUG("Saving project settings");
    const bool settingsSaved = SaveProjectSettings();

    // Update project file
    BE_DEBUG("Updating project file");
    nlohmann::json projectData;
    projectData["name"] = m_ProjectName;
    projectData["version"] = m_ProjectVersion;
    projectData["engine"] = "Black Engine";

    const std::string projectFilePath = m_ProjectPath + "/project.json";
    const bool projectSaved = FileSystem::WriteTextFile(projectFilePath, projectData.dump(4));

    if (projectSaved && settingsSaved) {
        BE_INFO(fmt::format("Project saved successfully: {}", m_ProjectName));
        return true;
    } else {
        BE_ERROR("Failed to save project");
        return false;
    }
}
