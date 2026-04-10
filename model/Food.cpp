#include "../model/Food.h"
#include <algorithm>
#include <sstream>

Food::Food(const std::string& id, const std::vector<std::string>& keywords)
    : id(id), keywords(keywords) {}

std::string Food::getId() const {
    return id;
}

std::vector<std::string> Food::getKeywords() const {
    return keywords;
}

void Food::addKeyword(const std::string& keyword) {
    // Check if keyword already exists
    if (std::find(keywords.begin(), keywords.end(), keyword) == keywords.end()) {
        keywords.push_back(keyword);
    }
}

bool Food::matchesAllKeywords(const std::vector<std::string>& searchKeywords) const {
    if (searchKeywords.empty()) {
        return true;
    }
    
    for (const auto& searchKeyword : searchKeywords) {
        bool found = false;
        for (const auto& keyword : keywords) {
            if (keyword.find(searchKeyword) != std::string::npos) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    return true;
}

bool Food::matchesAnyKeyword(const std::vector<std::string>& searchKeywords) const {
    if (searchKeywords.empty()) {
        return true;
    }
    
    for (const auto& searchKeyword : searchKeywords) {
        for (const auto& keyword : keywords) {
            if (keyword.find(searchKeyword) != std::string::npos) {
                return true;
            }
        }
    }
    
    return false;
}
