#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

/**
 * Utility class for string operations
 */
class StringUtils {
public:
    // Split a string by a delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Trim whitespace from beginning and end of string
    static std::string trim(const std::string& str);
    
    // Convert string to lowercase
    static std::string toLower(const std::string& str);
    
    // Convert string to uppercase
    static std::string toUpper(const std::string& str);
    
    // Check if a string starts with a prefix
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // Check if a string ends with a suffix
    static bool endsWith(const std::string& str, const std::string& suffix);
};

#endif // STRING_UTILS_H
