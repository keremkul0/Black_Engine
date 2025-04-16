#pragma once
#include "LogManager.h"

// Kategori tanımlama makrosu
// Kullanım: BE_DEFINE_LOG_CATEGORY(MainLog, "Main");
#define BE_DEFINE_LOG_CATEGORY(varName, catName) \
    static const std::string varName = catName

// Aşağıdaki makrolar tek tip kullanım sağlar

// Trace log
#define BE_LOG_TRACE(catVar, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().Log( \
        ::BlackEngine::LogLevel::Trace, \
        catVar, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )

// Debug log
#define BE_LOG_DEBUG(catVar, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().Log( \
        ::BlackEngine::LogLevel::Debug, \
        catVar, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )

// Info log
#define BE_LOG_INFO(catVar, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().Log( \
        ::BlackEngine::LogLevel::Info, \
        catVar, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )

// Warning log
#define BE_LOG_WARNING(catVar, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().Log( \
        ::BlackEngine::LogLevel::Warning, \
        catVar, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )

// Error log
#define BE_LOG_ERROR(catVar, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().Log( \
        ::BlackEngine::LogLevel::Error, \
        catVar, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )

// Critical log
#define BE_LOG_CRITICAL(catVar, fmt, ...) \
    ::BlackEngine::LogManager::GetInstance().Log( \
        ::BlackEngine::LogLevel::Critical, \
        catVar, \
        std::source_location::current(), \
        fmt, \
        ##__VA_ARGS__ \
    )
