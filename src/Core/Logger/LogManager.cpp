#include "LogManager.h"
#include "SpdlogBackend.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace BlackEngine {

//------------------------------------------------------------------------------
// LogBackendManager Implementation
//------------------------------------------------------------------------------

void LogBackendManager::AddBackend(const LogBackendPtr& backend) {
    if (!backend) return;
    
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    // Check if backend already exists
    for (const auto& existing : m_Backends) {
        if (existing->GetName() == backend->GetName()) {
            return;  // Backend with same name already exists
        }
    }
    
    m_Backends.push_back(backend);
}

void LogBackendManager::RemoveBackend(const std::string& backendName) {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    std::erase_if(m_Backends,
                  [&backendName](const LogBackendPtr& backend) {
                      return backend->GetName() == backendName;
                  });
}

void LogBackendManager::RemoveAllBackends() {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    m_Backends.clear();
}

LogBackendPtr LogBackendManager::GetBackend(const std::string& backendName) {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    for (const auto& backend : m_Backends) {
        if (backend->GetName() == backendName) {
            return backend;
        }
    }
    
    return nullptr;
}

std::vector<LogBackendPtr> LogBackendManager::GetAllBackends() {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    return m_Backends;
}

bool LogBackendManager::Initialize() {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    bool allInitialized = true;
    for (const auto& backend : m_Backends) {
        if (!backend->Initialize()) {
            allInitialized = false;
        }
    }
    
    return allInitialized;
}

void LogBackendManager::Shutdown() {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    for (const auto& backend : m_Backends) {
        backend->Shutdown();
    }
}

void LogBackendManager::Log(const LogMessage& message) {
    // Skip if below minimum level
    if (message.level < m_MinLogLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    for (const auto& backend : m_Backends) {
        if (backend->IsInitialized() && message.level >= backend->GetMinLogLevel()) {
            backend->Log(message);
        }
    }
}

void LogBackendManager::LogRepeat(const LogMessage& message, int count) {
    // Skip if below minimum level
    if (message.level < m_MinLogLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    for (const auto& backend : m_Backends) {
        if (backend->IsInitialized() && message.level >= backend->GetMinLogLevel()) {
            backend->LogRepeat(message, count);
        }
    }
}

void LogBackendManager::Flush() {
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    for (const auto& backend : m_Backends) {
        if (backend->IsInitialized()) {
            backend->Flush();
        }
    }
}

void LogBackendManager::SetMinLogLevel(LogLevel level) {
    m_MinLogLevel = level;
    
    std::lock_guard<std::mutex> lock(m_BackendMutex);
    
    for (const auto& backend : m_Backends) {
        backend->SetMinLogLevel(level);
    }
}

//------------------------------------------------------------------------------
// CategoryRegistry Implementation
//------------------------------------------------------------------------------

void CategoryRegistry::RegisterCategory(const std::string& name, LogLevel defaultLevel) {
    std::lock_guard<std::mutex> lock(m_CategoryMutex);
    
    // Create a new category if it doesn't exist
    if (!m_Categories.contains(name)) {
        // Use emplace to construct the CategoryInfo in-place
        // This avoids using the deleted copy assignment operator
        m_Categories.emplace(std::piecewise_construct,
                           std::forward_as_tuple(name),
                           std::forward_as_tuple(name, defaultLevel));
    }
}

CategoryInfo* CategoryRegistry::GetCategory(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_CategoryMutex);

    const auto it = m_Categories.find(name);
    if (it != m_Categories.end()) {
        return &it->second;
    }
    
    return nullptr;
}

std::vector<std::string> CategoryRegistry::GetAllCategoryNames() const {
    std::lock_guard<std::mutex> lock(m_CategoryMutex);
    
    std::vector<std::string> names;
    names.reserve(m_Categories.size());
    
    for (const auto &name: m_Categories | std::views::keys) {
        names.push_back(name);
    }
    
    return names;
}

bool CategoryRegistry::SetCategoryLevel(const std::string& name, LogLevel level) {
    std::lock_guard<std::mutex> lock(m_CategoryMutex);

    if (const auto it = m_Categories.find(name); it != m_Categories.end()) {
        it->second.level = level;
        return true;
    }
    
    return false;
}

bool CategoryRegistry::SetCategoryEnabled(const std::string& name, bool enabled) {
    std::lock_guard<std::mutex> lock(m_CategoryMutex);

    if (const auto it = m_Categories.find(name); it != m_Categories.end()) {
        it->second.enabled = enabled;
        return true;
    }
    
    return false;
}

bool CategoryRegistry::IsRepeatedMessage(const std::string& categoryName, 
                                        const std::string& message, 
                                        LogLevel level) {
    const LogMessageIdentifier id{categoryName, message};
    
    std::lock_guard<std::mutex> lock(m_RepeatMutex);

    if (const auto it = m_RepeatCounts.find(id); it != m_RepeatCounts.end() && it->second > 0) {
        return true;
    }
    
    return false;
}

void CategoryRegistry::UpdateRepeatCount(const std::string& categoryName, 
                                         const std::string& message, 
                                         LogLevel level) {
    const LogMessageIdentifier id{categoryName, message};
    
    std::lock_guard<std::mutex> lock(m_RepeatMutex);
    ++m_RepeatCounts[id];
}

int CategoryRegistry::GetAndResetRepeatCount(const std::string& categoryName, 
                                            const std::string& message, 
                                            LogLevel level) {
    const LogMessageIdentifier id{categoryName, message};
    
    std::lock_guard<std::mutex> lock(m_RepeatMutex);

    if (const auto it = m_RepeatCounts.find(id); it != m_RepeatCounts.end()) {
        const int count = it->second;
        m_RepeatCounts.erase(it);
        return count;
    }
    
    return 0;
}

//------------------------------------------------------------------------------
// LogManager Implementation
//------------------------------------------------------------------------------

LogManager& LogManager::GetInstance() {
    static LogManager instance;
    return instance;
}

LogManager::LogManager() : m_Initialized(false) {
}

LogManager::~LogManager() {
    Shutdown();
}

bool LogManager::Initialize() {
    std::lock_guard<std::mutex> lock(m_ConfigMutex);
    
    if (m_Initialized) {
        return true;
    }
    
    // If no backends, add the default spdlog backend
    if (m_BackendManager.GetAllBackends().empty()) {
        SpdlogBackend::Config config;
        const auto backend = std::make_shared<SpdlogBackend>(config);
        m_BackendManager.AddBackend(backend);
    }

    const bool initSuccess = m_BackendManager.Initialize();
    
    // Load config if available
    LoadConfig();
    
    m_Initialized = initSuccess;
      // Log initialization message
    if (m_Initialized) {
        const BlackEngine::LogMessage initMsg("Logging system initialized", LogLevel::Info, "LogManager");
        m_BackendManager.Log(initMsg);
    }
    
    return m_Initialized;
}

void LogManager::Shutdown() {
    std::lock_guard<std::mutex> lock(m_ConfigMutex);
    
    if (!m_Initialized) {
        return;
    }
    
    // Save config when shutting down
    SaveConfig();
      // Log shutdown message
    const BlackEngine::LogMessage shutdownMsg("Logging system shutting down", LogLevel::Info, "LogManager");
    m_BackendManager.Log(shutdownMsg);
    
    // Flush all logs
    m_BackendManager.Flush();
    
    // Shutdown all backends
    m_BackendManager.Shutdown();
    
    m_Initialized = false;
}

void LogManager::RegisterCategory(const std::string& name, LogLevel defaultLevel) {
    m_CategoryRegistry.RegisterCategory(name, defaultLevel);
}

CategoryInfo* LogManager::GetCategory(const std::string& name) {
    CategoryInfo* category = m_CategoryRegistry.GetCategory(name);
    
    // Create category if it doesn't exist
    if (!category) {
        RegisterCategory(name, m_DefaultLogLevel);
        category = m_CategoryRegistry.GetCategory(name);
    }
    
    return category;
}

bool LogManager::SetCategoryLevel(const std::string& name, LogLevel level) {
    return m_CategoryRegistry.SetCategoryLevel(name, level);
}

bool LogManager::SetCategoryEnabled(const std::string& name, bool enabled) {
    return m_CategoryRegistry.SetCategoryEnabled(name, enabled);
}

void LogManager::AddBackend(const LogBackendPtr& backend) {
    m_BackendManager.AddBackend(backend);
    
    // Initialize the backend if the logging system is already initialized
    if (m_Initialized && backend && !backend->IsInitialized()) {
        backend->Initialize();
    }
}

void LogManager::RemoveBackend(const std::string& backendName) {
    m_BackendManager.RemoveBackend(backendName);
}

LogBackendPtr LogManager::GetBackend(const std::string& backendName) {
    return m_BackendManager.GetBackend(backendName);
}

bool LogManager::LoadConfig(const std::string& configFile) {
    try {
        std::ifstream file(configFile);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON
        nlohmann::json config;
        file >> config;
        
        // Load default log level
        if (config.contains("defaultLogLevel")) {
            const auto levelStr = config["defaultLogLevel"].get<std::string>();
            SetDefaultLogLevel(StringToLogLevel(levelStr));
        }
        
        // Load category settings
        if (config.contains("categories")) {
            for (auto& [categoryName, categoryConfig] : config["categories"].items()) {
                // Register or update the category
                RegisterCategory(categoryName);
                
                if (categoryConfig.contains("level")) {
                    std::string levelStr = categoryConfig["level"].get<std::string>();
                    SetCategoryLevel(categoryName, StringToLogLevel(levelStr));
                }
                
                if (categoryConfig.contains("enabled")) {
                    bool enabled = categoryConfig["enabled"].get<bool>();
                    SetCategoryEnabled(categoryName, enabled);
                }
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        // Log the error directly
        std::cerr << "Failed to load logging config: " << e.what() << std::endl;
        return false;
    }
}

bool LogManager::SaveConfig(const std::string& configFile) {
    try {
        nlohmann::json config;
        
        // Save default log level
        config["defaultLogLevel"] = LogLevelToString(m_DefaultLogLevel);
        
        // Save category settings
        for (auto categoryNames = m_CategoryRegistry.GetAllCategoryNames(); const auto& name : categoryNames) {
            if (CategoryInfo* category = m_CategoryRegistry.GetCategory(name)) {
                config["categories"][name]["level"] = LogLevelToString(category->level);
                config["categories"][name]["enabled"] = category->enabled;
            }
        }
        
        // Save to file
        std::ofstream file(configFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(4) << config;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save logging config: " << e.what() << std::endl;
        return false;
    }
}

void LogManager::SetDefaultLogLevel(LogLevel level) {
    m_DefaultLogLevel = level;
    m_BackendManager.SetMinLogLevel(level);
}

void LogManager::HandleRepeatedLog(const BlackEngine::LogMessage& message) {
    // Check and track repeated logs
    const bool isRepeat = m_CategoryRegistry.IsRepeatedMessage(
        message.categoryName, message.message, message.level);
    
    if (isRepeat) {
        m_CategoryRegistry.UpdateRepeatCount(
            message.categoryName, message.message, message.level);
        
        // Output repeat message at threshold
        const int repeatCount = m_CategoryRegistry.GetAndResetRepeatCount(
            message.categoryName, message.message, message.level);
        
        if (repeatCount > 0 && repeatCount >= REPEAT_LOG_THRESHOLD) {
            m_BackendManager.LogRepeat(message, repeatCount + 1);  // +1 for current message
        } else {
            // Just log normally if under threshold
            m_BackendManager.Log(message);
        }
    } else {
        // Reset repeat counter and log
        m_CategoryRegistry.UpdateRepeatCount(
            message.categoryName, message.message, message.level);
        m_BackendManager.Log(message);
    }
}

void LogManager::Flush() {
    // Flush all backends
    m_BackendManager.Flush();
}

} // namespace BlackEngine
