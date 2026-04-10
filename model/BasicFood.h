#ifndef BASIC_FOOD_H
#define BASIC_FOOD_H

#include "Food.h"

/**
 * Represents a basic food item with fixed calorie content
 */
class BasicFood : public Food {
public:
    BasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories);
    
    // Accessors
    double getCaloriesPerServing() const override;
    
    // String representations
    std::string toString() const override;
    std::string toFileString() const override;
    
private:
    double caloriesPerServing;
};

#endif // BASIC_FOOD_H
