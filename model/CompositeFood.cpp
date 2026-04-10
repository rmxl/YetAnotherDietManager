#include "CompositeFood.h"
#include <sstream>

CompositeFood::CompositeFood(const std::string& id, const std::vector<std::string>& keywords)
    : Food(id, keywords) {}

void CompositeFood::addComponent(const FoodPtr& food, double servings) {
    components[food] = servings;
}

std::map<FoodPtr, double> CompositeFood::getComponents() const {
    return components;
}

double CompositeFood::getCaloriesPerServing() const {
    double totalCalories = 0.0;
    
    for (const auto& component : components) {
        FoodPtr food = component.first;
        double servings = component.second;
        
        totalCalories += food->getCaloriesPerServing() * servings;
    }
    
    return totalCalories;
}

std::string CompositeFood::toString() const {
    std::stringstream ss;
    ss << "Composite Food: " << id << " (" << getCaloriesPerServing() << " calories/serving)";
    ss << "\nKeywords: ";
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) {
            ss << ", ";
        }
    }
    
    ss << "\nComponents:";
    for (const auto& component : components) {
        ss << "\n  - " << component.first->getId() << " (" << component.second << " servings)";
    }
    
    return ss.str();
}

std::string CompositeFood::toFileString() const {
    std::stringstream ss;
    
    // Format: C|id|keyword1,keyword2,...|component1:servings,component2:servings,...
    ss << "C|" << id << "|";
    
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) {
            ss << ",";
        }
    }
    
    ss << "|";
    
    bool firstComponent = true;
    for (const auto& component : components) {
        if (!firstComponent) {
            ss << ",";
        }
        ss << component.first->getId() << ":" << component.second;
        firstComponent = false;
    }
    
    return ss.str();
}
