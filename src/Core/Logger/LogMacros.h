#pragma once

#include "LoggerManager.h"
#include <source_location>

//-----------------------------------------------------------------------------
// Core logging macros
//-----------------------------------------------------------------------------

// Basic logging macros
#define BE_TRACE(msg)       LoggerManager::GetLogger()->LogTrace(msg)
#define BE_DEBUG(msg)       LoggerManager::GetLogger()->LogDebug(msg)
#define BE_INFO(msg)        LoggerManager::GetLogger()->LogInfo(msg)
#define BE_WARN(msg)        LoggerManager::GetLogger()->LogWarning(msg)
#define BE_ERROR(msg)       LoggerManager::GetLogger()->LogError(msg)
#define BE_CRITICAL(msg)    LoggerManager::GetLogger()->LogCritical(msg)

// Formatted logging macros
#define BE_TRACE_FMT(fmt, ...)    LoggerManager::GetLogger()->LogTraceFormat(fmt, ##__VA_ARGS__)
#define BE_DEBUG_FMT(fmt, ...)    LoggerManager::GetLogger()->LogDebugFormat(fmt, ##__VA_ARGS__)
#define BE_INFO_FMT(fmt, ...)     LoggerManager::GetLogger()->LogInfoFormat(fmt, ##__VA_ARGS__)
#define BE_WARN_FMT(fmt, ...)     LoggerManager::GetLogger()->LogWarningFormat(fmt, ##__VA_ARGS__)
#define BE_ERROR_FMT(fmt, ...)    LoggerManager::GetLogger()->LogErrorFormat(fmt, ##__VA_ARGS__)
#define BE_CRITICAL_FMT(fmt, ...) LoggerManager::GetLogger()->LogCriticalFormat(fmt, ##__VA_ARGS__)

//-----------------------------------------------------------------------------
// Category-based logging macros
//-----------------------------------------------------------------------------

// Basic category logging macros
#define BE_CAT_TRACE(cat, msg)    LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogTrace(msg)
#define BE_CAT_DEBUG(cat, msg)    LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogDebug(msg)
#define BE_CAT_INFO(cat, msg)     LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogInfo(msg)
#define BE_CAT_WARN(cat, msg)     LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogWarning(msg)
#define BE_CAT_ERROR(cat, msg)    LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogError(msg)
#define BE_CAT_CRITICAL(cat, msg) LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogCritical(msg)

// Formatted category logging macros
#define BE_CAT_TRACE_FMT(cat, fmt, ...)    LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogTraceFormat(fmt, ##__VA_ARGS__)
#define BE_CAT_DEBUG_FMT(cat, fmt, ...)    LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogDebugFormat(fmt, ##__VA_ARGS__)
#define BE_CAT_INFO_FMT(cat, fmt, ...)     LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogInfoFormat(fmt, ##__VA_ARGS__)
#define BE_CAT_WARN_FMT(cat, fmt, ...)     LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogWarningFormat(fmt, ##__VA_ARGS__)
#define BE_CAT_ERROR_FMT(cat, fmt, ...)    LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogErrorFormat(fmt, ##__VA_ARGS__)
#define BE_CAT_CRITICAL_FMT(cat, fmt, ...) LoggerManager::GetLogger()->GetCategoryLogger(cat)->LogCriticalFormat(fmt, ##__VA_ARGS__)

//-----------------------------------------------------------------------------
// Conditional logging macros (compile-time)
//-----------------------------------------------------------------------------

// Define LOG_LEVEL_TRACE 0
// Define LOG_LEVEL_DEBUG 1
// Define LOG_LEVEL_INFO 2
// Define LOG_LEVEL_WARN 3
// Define LOG_LEVEL_ERROR 4
// Define LOG_LEVEL_CRITICAL 5
// Define LOG_LEVEL_OFF 6

#ifndef BE_LOG_LEVEL
    #ifdef BE_DEBUG_BUILD
        #define BE_LOG_LEVEL 0  // All logs in debug builds
    #else
        #define BE_LOG_LEVEL 2  // Only info and above in release builds
    #endif
#endif

// Conditional logging based on log level
#if BE_LOG_LEVEL <= 0
    #define BE_TRACE_ENABLED
#endif

#if BE_LOG_LEVEL <= 1
    #define BE_DEBUG_ENABLED
#endif

#if BE_LOG_LEVEL <= 2
    #define BE_INFO_ENABLED
#endif

#if BE_LOG_LEVEL <= 3
    #define BE_WARN_ENABLED
#endif

#if BE_LOG_LEVEL <= 4
    #define BE_ERROR_ENABLED
#endif

#if BE_LOG_LEVEL <= 5
    #define BE_CRITICAL_ENABLED
#endif

// Redefine macros to noop if disabled
#ifndef BE_TRACE_ENABLED
    #undef BE_TRACE
    #undef BE_TRACE_FMT
    #undef BE_CAT_TRACE
    #undef BE_CAT_TRACE_FMT
    #define BE_TRACE(msg) (void)0
    #define BE_TRACE_FMT(fmt, ...) (void)0
    #define BE_CAT_TRACE(cat, msg) (void)0
    #define BE_CAT_TRACE_FMT(cat, fmt, ...) (void)0
#endif

#ifndef BE_DEBUG_ENABLED
    #undef BE_DEBUG
    #undef BE_DEBUG_FMT
    #undef BE_CAT_DEBUG
    #undef BE_CAT_DEBUG_FMT
    #define BE_DEBUG(msg) (void)0
    #define BE_DEBUG_FMT(fmt, ...) (void)0
    #define BE_CAT_DEBUG(cat, msg) (void)0
    #define BE_CAT_DEBUG_FMT(cat, fmt, ...) (void)0
#endif

#ifndef BE_INFO_ENABLED
    #undef BE_INFO
    #undef BE_INFO_FMT
    #undef BE_CAT_INFO
    #undef BE_CAT_INFO_FMT
    #define BE_INFO(msg) (void)0
    #define BE_INFO_FMT(fmt, ...) (void)0
    #define BE_CAT_INFO(cat, msg) (void)0
    #define BE_CAT_INFO_FMT(cat, fmt, ...) (void)0
#endif

#ifndef BE_WARN_ENABLED
    #undef BE_WARN
    #undef BE_WARN_FMT
    #undef BE_CAT_WARN
    #undef BE_CAT_WARN_FMT
    #define BE_WARN(msg) (void)0
    #define BE_WARN_FMT(fmt, ...) (void)0
    #define BE_CAT_WARN(cat, msg) (void)0
    #define BE_CAT_WARN_FMT(cat, fmt, ...) (void)0
#endif

#ifndef BE_ERROR_ENABLED
    #undef BE_ERROR
    #undef BE_ERROR_FMT
    #undef BE_CAT_ERROR
    #undef BE_CAT_ERROR_FMT
    #define BE_ERROR(msg) (void)0
    #define BE_ERROR_FMT(fmt, ...) (void)0
    #define BE_CAT_ERROR(cat, msg) (void)0
    #define BE_CAT_ERROR_FMT(cat, fmt, ...) (void)0
#endif

#ifndef BE_CRITICAL_ENABLED
    #undef BE_CRITICAL
    #undef BE_CRITICAL_FMT
    #undef BE_CAT_CRITICAL
    #undef BE_CAT_CRITICAL_FMT
    #define BE_CRITICAL(msg) (void)0
    #define BE_CRITICAL_FMT(fmt, ...) (void)0
    #define BE_CAT_CRITICAL(cat, msg) (void)0
    #define BE_CAT_CRITICAL_FMT(cat, fmt, ...) (void)0
#endif

//-----------------------------------------------------------------------------
// File-level category definition
//-----------------------------------------------------------------------------

// Define a file-level logger category (place at top of file)
#define BE_DEFINE_LOG_CATEGORY(categoryName) \
    namespace { \
        const char* _BE_Current_Log_Category = categoryName; \
    }

// Category-based logging macros that use the file-level category
#define BE_CAT_TRACE_CURRENT(msg)     BE_CAT_TRACE(_BE_Current_Log_Category, msg)
#define BE_CAT_DEBUG_CURRENT(msg)     BE_CAT_DEBUG(_BE_Current_Log_Category, msg)
#define BE_CAT_INFO_CURRENT(msg)      BE_CAT_INFO(_BE_Current_Log_Category, msg)
#define BE_CAT_WARN_CURRENT(msg)      BE_CAT_WARN(_BE_Current_Log_Category, msg)
#define BE_CAT_ERROR_CURRENT(msg)     BE_CAT_ERROR(_BE_Current_Log_Category, msg)
#define BE_CAT_CRITICAL_CURRENT(msg)  BE_CAT_CRITICAL(_BE_Current_Log_Category, msg)

// Formatted category logging macros that use the file-level category
#define BE_CAT_TRACE_FMT_CURRENT(fmt, ...)    BE_CAT_TRACE_FMT(_BE_Current_Log_Category, fmt, ##__VA_ARGS__)
#define BE_CAT_DEBUG_FMT_CURRENT(fmt, ...)    BE_CAT_DEBUG_FMT(_BE_Current_Log_Category, fmt, ##__VA_ARGS__)
#define BE_CAT_INFO_FMT_CURRENT(fmt, ...)     BE_CAT_INFO_FMT(_BE_Current_Log_Category, fmt, ##__VA_ARGS__)
#define BE_CAT_WARN_FMT_CURRENT(fmt, ...)     BE_CAT_WARN_FMT(_BE_Current_Log_Category, fmt, ##__VA_ARGS__)
#define BE_CAT_ERROR_FMT_CURRENT(fmt, ...)    BE_CAT_ERROR_FMT(_BE_Current_Log_Category, fmt, ##__VA_ARGS__)
#define BE_CAT_CRITICAL_FMT_CURRENT(fmt, ...) BE_CAT_CRITICAL_FMT(_BE_Current_Log_Category, fmt, ##__VA_ARGS__)
