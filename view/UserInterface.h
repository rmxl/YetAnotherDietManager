#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "../controller/DietManager.h"
#include "../model/DailyLog.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

/**
 * Command-line user interface for the diet manager
 */
class UserInterface {
public:
    UserInterface(std::shared_ptr<DietManager> dietManager);
    
    // Run the application
    void run();
    
private:
    std::shared_ptr<DietManager> dietManager;
    Date currentDate;
    bool running;
    
    // Command handlers map
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commands;
    
    // Initialization methods
    void initializeCommandHandlers();
    void displayWelcome();
    void displayGoodbye();
    
    // Command processing
    void processCommand(const std::string& commandLine);
    std::vector<std::string> parseCommandLine(const std::string& commandLine);
    
    // Input handling
    std::string getInput(const std::string& prompt);
    int getIntInput(const std::string& prompt, int min, int max);
    double getDoubleInput(const std::string& prompt, double min, double max);
    bool getYesNoInput(const std::string& prompt);
    Date getDateInput(const std::string& prompt);
    
    // Command handlers
    void handleHelp();
    void handleExit();
    void handleLogin();
    void handleLogout();
    
    // Food management
    void handleAddFood();
    void addBasicFood();
    void addCompositeFood();
    void handleFindFood();
    void handleViewFood(const std::vector<std::string>& args);
    void handleEditFood(const std::vector<std::string>& args);
    void handleDeleteFood(const std::vector<std::string>& args);
    
    // Food logging
    void handleLogFood();
    void handleViewTodayLog();
    void handleViewLog(const std::vector<std::string>& args);
    void handleRemoveLogEntry(const std::vector<std::string>& args);
    
    // Profile management
    void handleViewProfile();
    void handleUpdateProfile();
    void handleSetTargetCalories();
    
    // Date management
    void handleSetDate(const std::vector<std::string>& args);
    
    // Command history
    void handleUndo();
    void handleRedo();
    
    // Helper methods
    void displayFoodList(const std::vector<FoodPtr>& foods);
    void displayDailyLog(const std::shared_ptr<DailyLog>& dailyLog, const Date& date);
    std::vector<std::string> parseKeywords(const std::string& keywordsString);
    bool isCircularReference(const std::shared_ptr<CompositeFood>& food, const std::string& targetId);
    std::vector<FoodPtr> searchFoodsHelper(const std::string& searchQuery, bool matchAll = false);
};

#endif // USER_INTERFACE_H
