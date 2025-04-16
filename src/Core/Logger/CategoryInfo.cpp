#include "CategoryInfo.h"

#include <utility>

namespace BlackEngine {

CategoryInfo::CategoryInfo(std::string  name, const LogLevel level)
    : m_name(std::move(name)), m_level(level) {
}

bool CategoryInfo::ShouldLog(const std::string& messageKey) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Mesaj sayacını güncelle
    auto& counter = m_messageCounters[messageKey];
    counter++;
    
    // Spam kontrol kurallarını uygula
    if (counter <= 10) {
        // 1-10: Her seferinde logla
        return true;
    } else if (counter <= 100) {
        // 11-100: Her 10 tekrarda bir logla
        return counter % 10 == 0;
    } else if (counter <= 500) {
        // 101-500: Her 50 tekrarda bir logla
        return counter % 50 == 0;
    } else if (counter <= 2000) {
        // 501-2000: Her 200 tekrarda bir logla
        return counter % 200 == 0;
    } else {
        // 2000+: Her 1000 tekrarda bir logla
        return counter % 1000 == 0;
    }
}

void CategoryInfo::ResetSpamControl() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_messageCounters.clear();
}

} // namespace BlackEngine
