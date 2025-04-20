#include "GuidUtils.h"

// Initialize static members
std::random_device GuidUtils::s_RandomDevice;
std::mt19937 GuidUtils::s_Generator(s_RandomDevice());
std::uniform_int_distribution<> GuidUtils::s_Distribution(0, 15);

/**
 * Generates a new RFC-4122 v4 compatible GUID.
 * @return A unique GUID string in the format xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 */
std::string GuidUtils::GenerateGuid() {
    // Create a buffer to build the GUID string
    std::stringstream ss;
    
    // Format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    // Where y is 8, 9, A, or B
    
    // Generate 32 hex digits with hyphens at specific positions
    for (int i = 0; i < 32; i++) {
        // Add hyphens at the right positions
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            ss << '-';
        }
        
        // Version 4 UUID has specific requirements:
        if (i == 12) {
            // The 13th digit must be '4' (version 4)
            ss << '4';
        } else if (i == 16) {
            // The 17th digit must be 8, 9, A, or B 
            // (Binary: 10xx, where x is random)
            const int digit = 8 + (s_Distribution(s_Generator) & 0x3);
            ss << std::hex << digit;
        } else {
            // Generate a random hex digit
            ss << std::hex << s_Distribution(s_Generator);
        }
    }
    
    return ss.str();
}
