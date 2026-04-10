#ifndef FOOD_DATABASE_H
#define FOOD_DATABASE_H

#include "BasicFood.h"
#include "CompositeFood.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

/**
 * Manages the database of food items (both basic and composite)
 */
class FoodDatabase {
public:
    FoodDatabase();
    ~FoodDatabase();
    
    // Methods for loading and saving database
    bool loadFromFiles(const std::string& basicFoodFile, const std::string& compositeFoodFile);
    bool saveToFiles(const std::string& basicFoodFile, const std::string& compositeFoodFile) const;
    
    // Methods for adding new foods
    bool addFood(const FoodPtr& food);
    void addBasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories);
    void addCompositeFood(const std::string& id, const std::vector<std::string>& keywords, 
                          const std::map<std::string, double>& componentServing);
    
    // Methods for updating and removing foods
    bool updateFood(const FoodPtr& food);
    bool removeFood(const std::string& id);
    
    // Retrieval methods
    FoodPtr getFoodById(const std::string& id) const;
    std::vector<FoodPtr> getAllFoods() const;
    std::vector<FoodPtr> searchFoods(const std::vector<std::string>& keywords, bool matchAll) const;
    
    // Check if food exists
    bool containsFood(const std::string& id) const;
    
    // Count methods
    int getBasicFoodCount() const;
    int getCompositeFoodCount() const;
    
private:
    // Maps to store foods by ID for quick lookup
    std::unordered_map<std::string, FoodPtr> foods;
};

typedef std::shared_ptr<FoodDatabase> FoodDatabasePtr;

#endif // FOOD_DATABASE_H
