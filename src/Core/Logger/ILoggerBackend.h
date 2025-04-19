#pragma once

#include "LogTypes.h"

namespace BlackEngine {
    /**
     * @brief Logger backend interface
     *
     * All log backends must implement this interface.
     */
    class ILoggerBackend {
    public:
        virtual ~ILoggerBackend() = default;

        /**
         * @brief Initializes the backend
         * @return Success status
         */
        virtual bool Initialize() = 0;

        /**
         * @brief Shuts down the backend
         */
        virtual void Shutdown() = 0;

        /**
         * @brief Processes the log message
         * @param message Log message
         */
        virtual void Log(const LogMessage &message) = 0;

        /**
         * @brief Checks whether the backend has been initialized
         * @return Backend status
         */
        [[nodiscard]] virtual bool IsInitialized() const = 0;
    };
} // namespace BlackEngine
