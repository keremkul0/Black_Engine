#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>

class GuidUtils {
public:
    /**
     * Generates a new RFC-4122 v4 compatible GUID.
     * @return A unique GUID string in the format xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
     */
    static std::string GenerateGuid();

private:
    // Random number generator and distribution
    static std::random_device s_RandomDevice;
    static std::mt19937 s_Generator;
    static std::uniform_int_distribution<> s_Distribution;
};
