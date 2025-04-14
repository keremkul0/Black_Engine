#pragma once

#include "LogManager.h"

// Convenience macro for logging
#define BE_LOG(level, category, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().LogMessage( \
        level, \
        category, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )

// Standard log macros
#define BE_LOG_TRACE(category, fmt, ...)   BE_LOG(::BlackEngine::LogLevel::Trace, category, fmt, ##__VA_ARGS__)
#define BE_LOG_DEBUG(category, fmt, ...)   BE_LOG(::BlackEngine::LogLevel::Debug, category, fmt, ##__VA_ARGS__)
#define BE_LOG_INFO(category, fmt, ...)    BE_LOG(::BlackEngine::LogLevel::Info, category, fmt, ##__VA_ARGS__)
#define BE_LOG_WARNING(category, fmt, ...) BE_LOG(::BlackEngine::LogLevel::Warning, category, fmt, ##__VA_ARGS__)
#define BE_LOG_ERROR(category, fmt, ...)   BE_LOG(::BlackEngine::LogLevel::Error, category, fmt, ##__VA_ARGS__)
#define BE_LOG_CRITICAL(category, fmt, ...)BE_LOG(::BlackEngine::LogLevel::Critical, category, fmt, ##__VA_ARGS__)

// Macro for defining a log category in a source file
#define BE_DEFINE_LOG_CATEGORY(varName, categoryName, defaultLevel) \
    const std::string varName = ([]() -> std::string { \
        ::BlackEngine::LogManager::GetInstance().RegisterCategory(categoryName, defaultLevel); \
        return categoryName; \
    })()

// Macro for declaring an extern log category in a header file
#define BE_DECLARE_LOG_CATEGORY(varName) \
    extern const std::string varName
