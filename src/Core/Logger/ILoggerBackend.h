#pragma once

#include "LogTypes.h"

namespace BlackEngine {

/**
 * @brief Logger backend arayüzü
 * 
 * Tüm log backend'leri bu arayüzü uygulamalıdır.
 */
class ILoggerBackend {
public:
    virtual ~ILoggerBackend() = default;
    
    /**
     * @brief Backend'i başlatır
     * @return Başarı durumu
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Backend'i kapatır
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Log mesajı işler
     * @param message Log mesajı
     */
    virtual void Log(const LogMessage& message) = 0;
    
    /**
     * @brief Backend'in başlatılıp başlatılmadığını kontrol eder
     * @return Backend durumu
     */
    [[nodiscard]] virtual bool IsInitialized() const = 0;
};

} // namespace BlackEngine
