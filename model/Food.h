#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <vector>
#include <memory>

/**
 * Abstract base class for all food items in the diet manager
 * Uses a composite pattern to allow for basic and composite foods
 */
class Food {
public:
    Food(const std::string& id, const std::vector<std::string>& keywords);
    virtual ~Food() = default;
    
    // Common accessors for all foods
    std::string getId() const;
    std::vector<std::string> getKeywords() const;
    void addKeyword(const std::string& keyword);
    
    // Virtual methods to be implemented by derived classes
    virtual double getCaloriesPerServing() const = 0;
    virtual std::string toString() const = 0;
    virtual std::string toFileString() const = 0;
    
    // Type checking methods for the composite pattern
    virtual bool isComposite() const { return false; }
    
    // Methods for keyword searching
    bool matchesAllKeywords(const std::vector<std::string>& searchKeywords) const;
    bool matchesAnyKeyword(const std::vector<std::string>& searchKeywords) const;
    
protected:
    std::string id;
    std::vector<std::string> keywords;
};

// Use smart pointers for managing food objects
typedef std::shared_ptr<Food> FoodPtr;

#endif // FOOD_H
