#include "MockLogger.h"

// Returns a mock category logger that also does nothing
std::shared_ptr<ILogger> MockLogger::GetCategoryLogger(const std::string& category) {
    // Simply return a new MockLogger instance for the category
    return std::make_shared<MockLogger>();
}