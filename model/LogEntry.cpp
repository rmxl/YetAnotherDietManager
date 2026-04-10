#include "LogEntry.h"
#include <sstream>
#include <iomanip>
#include "../util/StringUtils.h"

LogEntry::LogEntry(const FoodPtr& food, double servings, MealType mealType)
    : food(food), servings(servings), mealType(mealType) {}

FoodPtr LogEntry::getFood() const {
    return food;
}

double LogEntry::getServings() const {
    return servings;
}

double LogEntry::getCalories() const {
    return food->getCaloriesPerServing() * servings;
}

MealType LogEntry::getMealType() const {
    return mealType;
}

std::string LogEntry::getMealTypeString() const {
    return mealTypeToString(mealType);
}

std::string LogEntry::toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1)
       << food->getId() << " - " << servings << " serving(s), "
       << getCalories() << " calories (" << getMealTypeString() << ")";
    return ss.str();
}

std::string LogEntry::toFileString() const {
    std::stringstream ss;
    ss << food->getId() << "|" << servings << "|" << getMealTypeString();
    return ss.str();
}

std::shared_ptr<LogEntry> LogEntry::fromFileString(const std::string& fileString, const FoodPtr& food) {
    std::vector<std::string> parts = StringUtils::split(fileString, '|');
    
    if (parts.size() < 2) {
        return nullptr;
    }
    
    std::string foodId = parts[0];
    double servings = std::stod(parts[1]);
    
    MealType mealType = MealType::Other;
    if (parts.size() >= 3) {
        mealType = stringToMealType(parts[2]);
    }
    
    return std::make_shared<LogEntry>(food, servings, mealType);
}
