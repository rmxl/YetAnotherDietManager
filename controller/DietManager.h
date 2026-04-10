#ifndef DIET_MANAGER_H
#define DIET_MANAGER_H

#include "../model/FoodDatabase.h"
#include "../model/UserProfile.h"
#include "../model/DailyLog.h"
#include "../model/UserManager.h"
#include "../model/User.h"
#include "CommandManager.h"
#include "CalorieCalculator.h"
#include <memory>
#include <string>
#include <map>
#include <vector>

// Forward declarations
class Command;

/**
 * Main controller class that manages the diet tracking application
 */
class DietManager {
public:
    // Constructor and destructor
    DietManager();
    ~DietManager();
    
    // Inner classes for the Command pattern
    class AddFoodCommand : public Command {
    public:
        AddFoodCommand(DietManager* manager, const Date& date, 
                     const std::string& foodId, double servings, MealType mealType);
        virtual void execute() override;
        virtual void undo() override;
        virtual std::string getDescription() const override;
        
    private:
        DietManager* manager;
        Date date;
        std::string foodId;
        double servings;
        MealType mealType;
        int addedIndex;
    };
    
    class RemoveFoodCommand : public Command {
    public:
        RemoveFoodCommand(DietManager* manager, const Date& date, int entryIndex);
        virtual void execute() override;
        virtual void undo() override;
        virtual std::string getDescription() const override;
        
    private:
        DietManager* manager;
        Date date;
        int entryIndex;
        FoodPtr removedFood;
        double removedServings;
        MealType removedMealType;
    };
    
    class UpdateProfileCommand : public Command {
    public:
        UpdateProfileCommand(DietManager* manager, const std::string& name, 
                           Gender gender, int heightCm);
        virtual void execute() override;
        virtual void undo() override;
        virtual std::string getDescription() const override;
        
    private:
        DietManager* manager;
        std::string newName;
        Gender newGender;
        int newHeightCm;
        std::string oldName;
        Gender oldGender;
        int oldHeightCm;
    };
    
    class SetUserMetricsCommand : public Command {
    public:
        SetUserMetricsCommand(DietManager* manager, const Date& date, 
                            int age, double weight, ActivityLevel level);
        virtual void execute() override;
        virtual void undo() override;
        virtual std::string getDescription() const override;
        
    private:
        DietManager* manager;
        Date date;
        int newAge;
        double newWeight;
        ActivityLevel newLevel;
        int oldAge;
        double oldWeight;
        ActivityLevel oldLevel;
        bool hadPreviousMetrics;
    };
    
    class SetCalorieMethodCommand : public Command {
    public:
        SetCalorieMethodCommand(DietManager* manager, CalorieCalculationMethod method);
        virtual void execute() override;
        virtual void undo() override;
        virtual std::string getDescription() const override;
        
    private:
        DietManager* manager;
        CalorieCalculationMethod newMethod;
        CalorieCalculationMethod oldMethod;
    };
    
    // Initialization
    bool initialize(const std::string& dataPath);
    
    // User authentication methods
    bool loginUser(const std::string& username, const std::string& password);
    void logoutUser();
    bool isUserLoggedIn() const;
    bool isUsernameTaken(const std::string& username) const;
    bool registerUser(const std::string& username, const std::string& password);
    bool createUserWithProfile(const std::string& username, const std::string& password,
                           const std::string& name, Gender gender, int heightCm);
    UserPtr getCurrentUser() const;
    std::string getCurrentUsername() const;
    UserProfilePtr getUserProfile() const;
    std::vector<std::string> getAllUsernames() const;
    bool loadUserData(const std::string& username);
    
    // Food database methods
    FoodDatabasePtr getFoodDatabase() const;
    bool addBasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories);
    bool addCompositeFood(const std::string& id, const std::vector<std::string>& keywords, 
                     const std::map<std::string, double>& ingredients);
    std::vector<FoodPtr> searchFoods(const std::vector<std::string>& keywords, bool matchAll = false) const;
    
    // User profile methods
    void updateUserProfile(const std::string& name, Gender gender, int heightCm);
    void setUserMetrics(const Date& date, int weightKg, double bodyFatPct, ActivityLevel activityLevel);
    void setCalorieCalculationMethod(CalorieCalculationMethod method);
    double getTargetCalories(const Date& date) const;
    
    // Daily log methods
    DailyLogPtr getDailyLog(const Date& date);
    bool addFoodToLog(const Date& date, const std::string& foodId, double servings, MealType mealType = MealType::Other);
    bool removeFoodFromLog(const Date& date, int entryIndex);
    double getTotalCalories(const Date& date) const;
    double getRemainingCalories(const Date& date) const;
    
    // Command manager methods
    CommandManager* getCommandManager();
    bool undo();
    bool redo();
    bool canUndo() const;
    bool canRedo() const;
    
    // Save/Load methods
    bool saveAll() const;
    bool saveUserProfile();
    
    // File path getters
    std::string getBasicFoodFile() const;
    std::string getCompositeFoodFile() const;
    std::string getLogFile() const;
    std::string getProfileFile() const;
    std::string getUsersFile() const;
    
private:
    // Main application components
    FoodDatabasePtr foodDb;
    UserProfilePtr userProfile;
    std::map<Date, DailyLogPtr> dailyLogs;
    CommandManager commandManager;
    UserManagerPtr userManager;
    UserPtr currentUser;
    
    // File paths
    std::string dataPath;
    std::string basicFoodFile;
    std::string compositeFoodFile;
    std::string logFile;
    std::string profileFile;
    std::string usersFile;
    
    // Helper method to get user-specific file paths
    std::string getUserSpecificPath(const std::string& baseFileName) const;
};

#endif // DIET_MANAGER_H
