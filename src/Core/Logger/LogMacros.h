#pragma once

#include "Logger.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/logger.h"

// Core logger macros.
// These use the central logger without a specific category.
#define BE_TRACE(msg)        Logger::LogTrace(msg)
#define BE_DEBUG(msg)        Logger::LogDebug(msg)
#define BE_INFO(msg)         Logger::LogInfo(msg)
#define BE_WARN(msg)         Logger::LogWarning(msg)
#define BE_ERROR(msg)        Logger::LogError(msg)
#define BE_CRITICAL(msg)     Logger::LogCritical(msg)

// Core logger macros for formatted messages.
#define BE_TRACE_FORMAT(formatStr, ...)        Logger::LogTraceFormat(formatStr, __VA_ARGS__)
#define BE_DEBUG_FORMAT(formatStr, ...)        Logger::LogDebugFormat(formatStr, __VA_ARGS__)
#define BE_INFO_FORMAT(formatStr, ...)         Logger::LogInfoFormat(formatStr, __VA_ARGS__)
#define BE_WARN_FORMAT(formatStr, ...)         Logger::LogWarningFormat(formatStr, __VA_ARGS__)
#define BE_ERROR_FORMAT(formatStr, ...)        Logger::LogErrorFormat(formatStr, __VA_ARGS__)
#define BE_CRITICAL_FORMAT(formatStr, ...)     Logger::LogCriticalFormat(formatStr, __VA_ARGS__)

// Category-based logger macros.
// These specify the category each time you log.
#define BE_CAT_TRACE(category, msg)            Logger::GetCategoryLogger(category)->trace(msg)
#define BE_CAT_DEBUG(category, msg)            Logger::GetCategoryLogger(category)->debug(msg)
#define BE_CAT_INFO(category, msg)             Logger::GetCategoryLogger(category)->info(msg)
#define BE_CAT_WARN(category, msg)             Logger::GetCategoryLogger(category)->warn(msg)
#define BE_CAT_ERROR(category, msg)            Logger::GetCategoryLogger(category)->error(msg)
#define BE_CAT_CRITICAL(category, msg)         Logger::GetCategoryLogger(category)->critical(msg)

// Category-based logger macros with formatted messages.
#define BE_CAT_TRACE_FORMAT(category, formatStr, ...)    Logger::GetCategoryLogger(category)->trace(fmt::format(formatStr, __VA_ARGS__))
#define BE_CAT_DEBUG_FORMAT(category, formatStr, ...)    Logger::GetCategoryLogger(category)->debug(fmt::format(formatStr, __VA_ARGS__))
#define BE_CAT_INFO_FORMAT(category, formatStr, ...)     Logger::GetCategoryLogger(category)->info(fmt::format(formatStr, __VA_ARGS__))
#define BE_CAT_WARN_FORMAT(category, formatStr, ...)     Logger::GetCategoryLogger(category)->warn(fmt::format(formatStr, __VA_ARGS__))
#define BE_CAT_ERROR_FORMAT(category, formatStr, ...)    Logger::GetCategoryLogger(category)->error(fmt::format(formatStr, __VA_ARGS__))
#define BE_CAT_CRITICAL_FORMAT(category, formatStr, ...) Logger::GetCategoryLogger(category)->critical(fmt::format(formatStr, __VA_ARGS__))

// Conditional logging macro.
// Logs a message only if 'cond' is true, using the specified LogLevel.
#define BE_LOG_IF(cond, level, msg) \
    do { \
        if (cond) { \
            Logger::Log(level, msg); \
        } \
    } while (0)

// Macro to execute code or log only in debug builds.
#ifdef _DEBUG
    #define BE_DEBUG_ONLY(x) x
#else
    #define BE_DEBUG_ONLY(x)
#endif
