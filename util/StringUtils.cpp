#include "StringUtils.h"
#include <algorithm>
#include <sstream>
#include <cctype>

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string StringUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    
    return str.substr(0, prefix.length()) == prefix;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    
    return str.substr(str.length() - suffix.length()) == suffix;
}
