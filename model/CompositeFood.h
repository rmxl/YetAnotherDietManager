#ifndef COMPOSITE_FOOD_H
#define COMPOSITE_FOOD_H

#include "Food.h"
#include <map>

/**
 * Represents a composite food made up of other foods
 */
class CompositeFood : public Food {
public:
    CompositeFood(const std::string& id, const std::vector<std::string>& keywords);
    
    // Methods for managing component foods
    void addComponent(const FoodPtr& food, double servings);
    std::map<FoodPtr, double> getComponents() const;
    
    // Override methods from base class
    double getCaloriesPerServing() const override;
    std::string toString() const override;
    std::string toFileString() const override;
    
    // Override the isComposite method
    bool isComposite() const override { return true; }
    
private:
    // Map of component foods and their serving amounts
    std::map<FoodPtr, double> components;
};

typedef std::shared_ptr<CompositeFood> CompositeFoodPtr;

#endif // COMPOSITE_FOOD_H
