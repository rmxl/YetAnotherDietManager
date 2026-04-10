#include "FoodDatabase.h"
#include "../util/FileHandler.h"
#include "../util/StringUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

FoodDatabase::FoodDatabase() {}

FoodDatabase::~FoodDatabase() {}

bool FoodDatabase::loadFromFiles(const std::string& basicFoodFile, const std::string& compositeFoodFile) {
    // Clear existing data
    foods.clear();
    
    // Load basic foods first
    std::vector<std::string> basicFoodLines;
    if (!FileHandler::readLines(basicFoodFile, basicFoodLines)) {
        std::cerr << "Error reading basic food file: " << basicFoodFile << std::endl;
        return false;
    }
    
    for (const auto& line : basicFoodLines) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }
        
        std::vector<std::string> parts = StringUtils::split(line, '|');
        if (parts.size() >= 4 && parts[0] == "B") {
            std::string id = parts[1];
            double calories = std::stod(parts[2]);
            std::vector<std::string> keywords = StringUtils::split(parts[3], ',');
            
            addBasicFood(id, keywords, calories);
        }
    }
    
    // Load composite foods second (since they may reference basic foods)
    std::vector<std::string> compositeFoodLines;
    if (!FileHandler::readLines(compositeFoodFile, compositeFoodLines)) {
        std::cerr << "Error reading composite food file: " << compositeFoodFile << std::endl;
        return false;
    }
    
    for (const auto& line : compositeFoodLines) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }
        
        std::vector<std::string> parts = StringUtils::split(line, '|');
        if (parts.size() >= 4 && parts[0] == "C") {
            std::string id = parts[1];
            std::vector<std::string> keywords = StringUtils::split(parts[2], ',');
            
            // Parse component foods and their servings
            std::map<std::string, double> componentServing;
            std::vector<std::string> components = StringUtils::split(parts[3], ',');
            
            for (const auto& component : components) {
                std::vector<std::string> componentParts = StringUtils::split(component, ':');
                if (componentParts.size() == 2) {
                    std::string componentId = componentParts[0];
                    double servings = std::stod(componentParts[1]);
                    componentServing[componentId] = servings;
                }
            }
            
            addCompositeFood(id, keywords, componentServing);
        }
    }
    
    return true;
}

bool FoodDatabase::saveToFiles(const std::string& basicFoodFile, const std::string& compositeFoodFile) const {
    std::vector<std::string> basicFoodLines;
    std::vector<std::string> compositeFoodLines;
    
    // Header comments
    basicFoodLines.push_back("# Basic Foods Database");
    basicFoodLines.push_back("# Format: B|id|calories|keyword1,keyword2,...");
    compositeFoodLines.push_back("# Composite Foods Database");
    compositeFoodLines.push_back("# Format: C|id|keyword1,keyword2,...|component1:servings,component2:servings,...");
    
    // Organize foods into basic and composite
    for (const auto& pair : foods) {
        FoodPtr food = pair.second;
        
        // Check if it's a basic food
        std::shared_ptr<BasicFood> basicFood = std::dynamic_pointer_cast<BasicFood>(food);
        if (basicFood) {
            basicFoodLines.push_back(basicFood->toFileString());
            continue;
        }
        
        // Check if it's a composite food
        std::shared_ptr<CompositeFood> compositeFood = std::dynamic_pointer_cast<CompositeFood>(food);
        if (compositeFood) {
            compositeFoodLines.push_back(compositeFood->toFileString());
            continue;
        }
    }
    
    // Save to files
    if (!FileHandler::writeLines(basicFoodFile, basicFoodLines)) {
        std::cerr << "Error writing basic food file: " << basicFoodFile << std::endl;
        return false;
    }
    
    if (!FileHandler::writeLines(compositeFoodFile, compositeFoodLines)) {
        std::cerr << "Error writing composite food file: " << compositeFoodFile << std::endl;
        return false;
    }
    
    return true;
}

void FoodDatabase::addBasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories) {
    FoodPtr basicFood = std::make_shared<BasicFood>(id, keywords, calories);
    foods[id] = basicFood;
}

void FoodDatabase::addCompositeFood(const std::string& id, const std::vector<std::string>& keywords, 
                                  const std::map<std::string, double>& componentServing) {
    CompositeFoodPtr compositeFood = std::make_shared<CompositeFood>(id, keywords);
    
    // Add components
    for (const auto& pair : componentServing) {
        std::string componentId = pair.first;
        double servings = pair.second;
        
        // Find the component food
        FoodPtr componentFood = getFoodById(componentId);
        
        if (componentFood) {
            compositeFood->addComponent(componentFood, servings);
        }
        else {
            std::cerr << "Warning: Component food '" << componentId 
                      << "' not found when creating composite food '" << id << "'" << std::endl;
        }
    }
    
    foods[id] = compositeFood;
}

FoodPtr FoodDatabase::getFoodById(const std::string& id) const {
    auto it = foods.find(id);
    if (it != foods.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<FoodPtr> FoodDatabase::getAllFoods() const {
    std::vector<FoodPtr> result;
    result.reserve(foods.size());
    
    for (const auto& pair : foods) {
        result.push_back(pair.second);
    }
    
    // Sort by ID for consistent output
    std::sort(result.begin(), result.end(), 
              [](const FoodPtr& a, const FoodPtr& b) { return a->getId() < b->getId(); });
    
    return result;
}

std::vector<FoodPtr> FoodDatabase::searchFoods(const std::vector<std::string>& keywords, bool matchAll) const {
    std::vector<FoodPtr> result;
    
    for (const auto& pair : foods) {
        FoodPtr food = pair.second;
        
        if ((matchAll && food->matchesAllKeywords(keywords)) || 
            (!matchAll && food->matchesAnyKeyword(keywords))) {
            result.push_back(food);
        }
    }
    
    // Sort by ID for consistent output
    std::sort(result.begin(), result.end(), 
              [](const FoodPtr& a, const FoodPtr& b) { return a->getId() < b->getId(); });
    
    return result;
}

bool FoodDatabase::containsFood(const std::string& id) const {
    return foods.find(id) != foods.end();
}

int FoodDatabase::getBasicFoodCount() const {
    int count = 0;
    for (const auto& pair : foods) {
        if (std::dynamic_pointer_cast<BasicFood>(pair.second)) {
            count++;
        }
    }
    return count;
}

int FoodDatabase::getCompositeFoodCount() const {
    int count = 0;
    for (const auto& pair : foods) {
        if (std::dynamic_pointer_cast<CompositeFood>(pair.second)) {
            count++;
        }
    }
    return count;
}
bool FoodDatabase::addFood(const FoodPtr& food) {
    if (!food) {
        return false;
    }
    
    // Check if the food already exists
    std::string id = food->getId();
    if (containsFood(id)) {
        return false; // Food with this ID already exists
    }
    
    foods[id] = food;
    return true;
}

bool FoodDatabase::updateFood(const FoodPtr& food) {
    if (!food) {
        return false;
    }
    
    std::string id = food->getId();
    if (!containsFood(id)) {
        return false; // Food with this ID doesn't exist
    }
    
    foods[id] = food;
    return true;
}

bool FoodDatabase::removeFood(const std::string& id) {
    auto it = foods.find(id);
    if (it == foods.end()) {
        return false; // Food not found
    }
    
    foods.erase(it);
    return true;
}
