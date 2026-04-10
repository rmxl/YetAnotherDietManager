#include "BasicFood.h"
#include <sstream>

BasicFood::BasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories)
    : Food(id, keywords), caloriesPerServing(calories) {}

double BasicFood::getCaloriesPerServing() const {
    return caloriesPerServing;
}

std::string BasicFood::toString() const {
    std::stringstream ss;
    ss << "Basic Food: " << id << " (" << caloriesPerServing << " calories/serving)";
    ss << "\nKeywords: ";
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) {
            ss << ", ";
        }
    }
    
    return ss.str();
}

std::string BasicFood::toFileString() const {
    std::stringstream ss;
    
    // Format: B|id|calories|keyword1,keyword2,...
    ss << "B|" << id << "|" << caloriesPerServing << "|";
    
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) {
            ss << ",";
        }
    }
    
    return ss.str();
}
