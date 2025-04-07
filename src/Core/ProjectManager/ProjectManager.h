#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

using json = nlohmann::json;

// Forward declaration
class AssetDatabase;

struct ProjectSettings {
    struct BuildSettings {
        std::string outputDirectory = "Build";
        std::string buildType = "Debug";
    };

    struct EditorSettings {
        int autoSaveInterval = 300;
        std::string theme = "Dark";
        bool showGrid = true;
    };

    struct RenderSettings {
        int msaaLevel = 4;
        std::string shadowQuality = "Medium";
        bool useHDR = true;
    };

    BuildSettings buildSettings;
    EditorSettings editorSettings;
    RenderSettings renderSettings;

    bool vsync = true;
    int msaa = 4;
    glm::ivec2 resolution = {1280, 720};
    std::string editorLayout = "default";
};

class ProjectManager {
public:
    static ProjectManager& GetInstance();

    bool LoadProject(const std::string& projectPath);
    bool LoadProjectSettings();
    bool SaveProject();
    [[nodiscard]] std::string GetProjectPath() const;

    // Added missing methods
    void CreateDefaultSettings();
    bool SaveProjectSettings();

    // Getters for settings
    [[nodiscard]] const ProjectSettings& GetSettings() const { return m_Settings; }
    ProjectSettings& GetMutableSettings() { return m_Settings; }

private:
    ProjectManager() = default;
    ~ProjectManager() = default;

    std::string m_ProjectPath;
    std::string m_ProjectName;
    std::string m_ProjectVersion;
    ProjectSettings m_Settings;
};
