#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include "Food.h"
#include <memory>
#include <string>

// Define meal types
enum class MealType {
    Breakfast,
    Lunch,
    Dinner,
    Snack,
    Other
};

// Convert MealType enum to string
inline std::string mealTypeToString(MealType mealType) {
    switch (mealType) {
        case MealType::Breakfast: return "Breakfast";
        case MealType::Lunch: return "Lunch";
        case MealType::Dinner: return "Dinner";
        case MealType::Snack: return "Snack";
        case MealType::Other: 
        default: return "Other";
    }
}

// Convert string to MealType enum
inline MealType stringToMealType(const std::string& mealTypeStr) {
    if (mealTypeStr == "Breakfast") return MealType::Breakfast;
    if (mealTypeStr == "Lunch") return MealType::Lunch;
    if (mealTypeStr == "Dinner") return MealType::Dinner;
    if (mealTypeStr == "Snack") return MealType::Snack;
    return MealType::Other;
}

/**
 * Represents a single food entry in the daily log
 */
class LogEntry {
public:
    LogEntry(const FoodPtr& food, double servings, MealType mealType = MealType::Other);
    
    // Accessors
    FoodPtr getFood() const;
    double getServings() const;
    double getCalories() const;
    MealType getMealType() const;
    std::string getMealTypeString() const;
    
    // String representation
    std::string toString() const;
    std::string toFileString() const;
    
    // Static method to create from file string
    static std::shared_ptr<LogEntry> fromFileString(const std::string& fileString, const FoodPtr& food);
    
private:
    FoodPtr food;
    double servings;
    MealType mealType;
};

typedef std::shared_ptr<LogEntry> LogEntryPtr;

#endif // LOG_ENTRY_H
