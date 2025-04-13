#pragma once

#include "ILogger.h"
#include "SpdlogLogger.h"
#include "MockLogger.h"
#include <memory>
#include <iostream>
#include <filesystem>

// LoggerManager provides global access to a logger instance and manages its lifetime
class LoggerManager {
public:
    // Get the current global logger instance
    static std::shared_ptr<ILogger> GetLogger() {
        return s_Logger;
    }
    
    // Set the global logger instance
    static void SetLogger(const std::shared_ptr<ILogger> &logger) {
        s_Logger = logger;
    }    // Initialize the global logger with the default implementation 
    // based on whether we're in a test environment or not
    static void Initialize(const bool isTestEnvironment = false) {
        // Create a new logger instance if one doesn't exist
        if (!s_Logger) {
            try {
                if (isTestEnvironment) {
                    // In test environment, use a MockLogger by default
                    s_Logger = std::make_shared<MockLogger>();
                    s_Logger->Initialize();
                    // No log message to keep tests clean
                } else {
                    // Production environment uses SpdlogLogger
                    s_Logger = std::make_shared<SpdlogLogger>();
                    s_Logger->Initialize();
                    s_Logger->LogInfo("Logger initialized in production environment");
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
                // Create a basic fallback logger in case of initialization failure
                s_Logger = std::make_shared<SpdlogLogger>();
            }
        }
    }
      // Initialize the logger with configuration from a JSON file
    static void InitializeFromJson(const std::string& jsonConfigPath, const bool isTestEnvironment = false) {
        // Create a new logger instance if one doesn't exist
        if (!s_Logger) {
            try {
                if (isTestEnvironment) {
                    // In test environment, use a MockLogger by default
                    s_Logger = std::make_shared<MockLogger>();
                    s_Logger->Initialize();
                    // No log message to keep tests clean
                } else {
                    // Production environment uses SpdlogLogger with JSON config
                    auto logger = std::make_shared<SpdlogLogger>();
                      // First set the logger to avoid init messages going nowhere
                    s_Logger = logger;
                    
                    // Then initialize with JSON configuration
                    // This order prevents deadlock since we already have s_Logger set
                    if (std::filesystem::exists(jsonConfigPath)) {
                        logger->Initialize(jsonConfigPath);
                    } else {
                        std::cerr << "Config file not found: " << jsonConfigPath << std::endl;
                        logger->Initialize(); // Fall back to standard initialization
                        logger->LogWarning("Config file not found, using default settings: " + jsonConfigPath,
                                          std::source_location::current());
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to initialize logger from JSON: " << e.what() << std::endl;
                // Fall back to regular initialization
                if (!s_Logger) { // Only if the logger wasn't set during the try block
                    Initialize(isTestEnvironment);
                }
            }
        }
    }
    
    // Shutdown the logger
    static void Shutdown() {
        if (s_Logger) {
            s_Logger->LogInfo("Shutting down logger");
            s_Logger->Shutdown();
            s_Logger.reset();
        }
    }

private:
    // Private static field to hold the current logger instance
    static std::shared_ptr<ILogger> s_Logger;
};