#pragma once

#include "LogTypes.h"
#include <memory>

namespace BlackEngine {

/**
 * @brief Interface for log backend implementations
 * 
 * This interface allows for different logging implementations (like spdlog, custom backends, etc.)
 * to be used with the logging system.
 */
class ILogBackend {
public:
    virtual ~ILogBackend() = default;
    
    /**
     * @brief Initialize the logging backend
     * @return true if initialization was successful
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Shut down the logging backend
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Log a message
     * @param message The log message to be logged
     */
    virtual void Log(const LogMessage& message) = 0;
    
    /**
     * @brief Log a repeat message (when the same message is logged multiple times)
     * @param message The original log message
     * @param count Number of times the message was repeated
     */
    virtual void LogRepeat(const LogMessage& message, int count) = 0;
    
    /**
     * @brief Flush any pending log messages
     */
    virtual void Flush() = 0;
    
    /**
     * @brief Check if this backend is initialized
     */
    [[nodiscard]] virtual bool IsInitialized() const = 0;
    
    /**
     * @brief Set the minimum log level for this backend
     */
    virtual void SetMinLogLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level for this backend
     */
    [[nodiscard]] virtual LogLevel GetMinLogLevel() const = 0;
    
    /**
     * @brief Get the name of this backend
     */
    [[nodiscard]] virtual std::string GetName() const = 0;
};

using LogBackendPtr = std::shared_ptr<ILogBackend>;

} // namespace BlackEngine
