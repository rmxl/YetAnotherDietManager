#include "DietManager.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include <filesystem>
#include <algorithm> // Add this for std::find

// --- DietManager Implementation ---

DietManager::DietManager() {}

DietManager::~DietManager()
{
    // Save data on exit
    saveAll();
}

bool DietManager::initialize(const std::string &dataPath)
{
    this->dataPath = dataPath;
// Create data directory if it doesn't exist
#ifdef _WIN32
    _mkdir(dataPath.c_str());
#else
    mkdir(dataPath.c_str(), 0777);
#endif
    std::filesystem::create_directories(dataPath);

    // Set file paths
    basicFoodFile = dataPath + "/basic_foods.txt";
    compositeFoodFile = dataPath + "/composite_foods.txt";
    usersFile = dataPath + "/users.txt";

    // Initialize food database
    foodDb = std::make_shared<FoodDatabase>();
    if (!foodDb->loadFromFiles(basicFoodFile, compositeFoodFile))
    {
        std::cerr << "Failed to load food database. Creating new database." << std::endl;

        // Create sample foods if database doesn't exist
        if (foodDb->getBasicFoodCount() == 0)
        {
            // Add some sample basic foods
            addBasicFood("Apple", {"apple", "fruit"}, 95);
            addBasicFood("Banana", {"banana", "fruit"}, 105);
            addBasicFood("ChickenBreast", {"chicken", "meat", "protein"}, 165);
            addBasicFood("WhiteBread", {"bread", "grain"}, 75);
            addBasicFood("PeanutButter", {"peanut", "butter", "spread"}, 190);
            addBasicFood("Jelly", {"jam", "jelly", "spread"}, 50);
            addBasicFood("Milk", {"milk", "dairy"}, 120);
            addBasicFood("Cheese", {"cheese", "dairy"}, 110);
            addBasicFood("Pasta", {"pasta", "grain"}, 200);
            addBasicFood("Rice", {"rice", "grain"}, 205);
            addBasicFood("Egg", {"egg", "protein"}, 70);
            addBasicFood("Potato", {"potato", "vegetable"}, 160);

            // Create sample composite foods
            std::map<std::string, double> pbSandwich = {
                {"WhiteBread", 2.0},
                {"PeanutButter", 1.0}};
            addCompositeFood("PBSandwich", {"sandwich", "peanut butter"}, pbSandwich);

            std::map<std::string, double> pbJSandwich = {
                {"PBSandwich", 1.0},
                {"Jelly", 1.0}};
            addCompositeFood("PBJSandwich", {"sandwich", "peanut butter", "jelly"}, pbJSandwich);

            // Save the sample database
            foodDb->saveToFiles(basicFoodFile, compositeFoodFile);
        }
    }

    // Initialize user manager
    userManager = std::make_shared<UserManager>();
    if (!userManager->loadFromFile(usersFile))
    {
        std::cerr << "Failed to load user database. Creating new database." << std::endl;

        // Add a default user if no users exist
        if (userManager->getAllUsernames().empty())
        {
            registerUser("default", "password");
            userManager->saveToFile(usersFile);
        }
    }

    // User profile and logs will be loaded when a user logs in
    userProfile = nullptr;
    dailyLogs.clear();

    return true;
}

bool DietManager::registerUser(const std::string &username, const std::string &password)
{
    return userManager->addUser(username, password);
}

bool DietManager::loginUser(const std::string &username, const std::string &password)
{
    if (userManager->loginUser(username, password))
    {
        // Load user-specific data after successful login
        return loadUserData(username);
    }
    return false;
}

void DietManager::logoutUser()
{
    // Save data before logout
    saveAll();

    // Clear current user data
    userManager->logoutCurrentUser();
    userProfile = nullptr;
    dailyLogs.clear();
}

bool DietManager::isUserLoggedIn() const
{
    return userManager->isUserLoggedIn();
}

std::string DietManager::getCurrentUsername() const
{
    if (userManager->getCurrentUser())
    {
        return userManager->getCurrentUser()->getUsername();
    }
    return "";
}

std::vector<std::string> DietManager::getAllUsernames() const
{
    return userManager->getAllUsernames();
}

bool DietManager::loadUserData(const std::string &username)
{
    // Set user-specific file paths
    profileFile = getUserSpecificPath("profile_" + username + ".txt");
    logFile = getUserSpecificPath("logs_" + username + ".txt");

    // Load user profile
    userProfile = UserProfile::loadFromFile(profileFile);
    if (!userProfile)
    {
        userProfile = std::make_shared<UserProfile>();
        userProfile->saveToFile(profileFile);
    }

    // Load daily logs
    dailyLogs = DailyLog::loadFromFile(logFile, foodDb);

    return true;
}

std::string DietManager::getUserSpecificPath(const std::string &baseFileName) const
{
    return dataPath + "/" + baseFileName;
}

FoodDatabasePtr DietManager::getFoodDatabase() const
{
    return foodDb;
}

bool DietManager::addBasicFood(const std::string &id, const std::vector<std::string> &keywords, double calories)
{
    if (foodDb->containsFood(id))
    {
        return false;
    }

    foodDb->addBasicFood(id, keywords, calories);
    return true;
}

bool DietManager::addCompositeFood(const std::string &id, const std::vector<std::string> &keywords,
                                   const std::map<std::string, double> &componentServing)
{
    if (foodDb->containsFood(id))
    {
        return false;
    }

    foodDb->addCompositeFood(id, keywords, componentServing);
    return true;
}

std::vector<FoodPtr> DietManager::searchFoods(const std::vector<std::string> &keywords, bool matchAll) const
{
    return foodDb->searchFoods(keywords, matchAll);
}

UserProfilePtr DietManager::getUserProfile() const
{
    return userProfile;
}

void DietManager::updateUserProfile(const std::string &name, Gender gender, int heightCm)
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return;
    }

    CommandPtr command = std::make_shared<UpdateProfileCommand>(this, name, gender, heightCm);
    commandManager.executeCommand(command);
}

void DietManager::setUserMetrics(const Date &date, int age, double weight, ActivityLevel level)
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return;
    }

    CommandPtr command = std::make_shared<SetUserMetricsCommand>(this, date, age, weight, level);
    commandManager.executeCommand(command);
}

void DietManager::setCalorieCalculationMethod(CalorieCalculationMethod method)
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return;
    }

    CommandPtr command = std::make_shared<SetCalorieMethodCommand>(this, method);
    commandManager.executeCommand(command);
}

double DietManager::getTargetCalories(const Date &date) const
{
    if (!isUserLoggedIn() || !userProfile)
    {
        return 0.0;
    }
    return userProfile->getTargetCalories(date);
}

DailyLogPtr DietManager::getDailyLog(const Date &date)
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return nullptr;
    }

    auto it = dailyLogs.find(date);
    if (it == dailyLogs.end())
    {
        // Create a new log for this date
        DailyLogPtr log = std::make_shared<DailyLog>(date);
        dailyLogs[date] = log;
        return log;
    }
    return it->second;
}

bool DietManager::addFoodToLog(const Date &date, const std::string &foodId, double servings, MealType mealType)
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return false;
    }

    CommandPtr command = std::make_shared<AddFoodCommand>(this, date, foodId, servings, mealType);
    commandManager.executeCommand(command);
    return true;
}

bool DietManager::removeFoodFromLog(const Date &date, int entryIndex)
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return false;
    }

    CommandPtr command = std::make_shared<RemoveFoodCommand>(this, date, entryIndex);
    commandManager.executeCommand(command);
    return true;
}

double DietManager::getTotalCalories(const Date &date) const
{
    if (!isUserLoggedIn())
    {
        return 0.0;
    }

    auto it = dailyLogs.find(date);
    if (it == dailyLogs.end())
    {
        return 0.0;
    }
    return it->second->getTotalCalories();
}

double DietManager::getRemainingCalories(const Date &date) const
{
    if (!isUserLoggedIn())
    {
        return 0.0;
    }

    double target = getTargetCalories(date);
    double consumed = getTotalCalories(date);
    return target - consumed;
}

bool DietManager::canUndo() const
{
    return isUserLoggedIn() && commandManager.canUndo();
}

CommandManager *DietManager::getCommandManager()
{
    return &commandManager;
}

bool DietManager::undo()
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return false;
    }

    return commandManager.undo();
}

bool DietManager::redo()
{
    if (!isUserLoggedIn())
    {
        std::cerr << "Error: User not logged in" << std::endl;
        return false;
    }

    return commandManager.redo();
}

bool DietManager::canRedo() const
{
    return isUserLoggedIn() && commandManager.canRedo();
}

bool DietManager::saveAll() const
{
    bool success = true;
    // Save user accounts
    if (!userManager->saveToFile(usersFile))
    {
        std::cerr << "Failed to save user accounts" << std::endl;
        success = false;
    }

    // Save user-specific data if logged in
    if (isUserLoggedIn() && userProfile)
    {
        if (!userProfile->saveToFile(profileFile))
        {
            std::cerr << "Failed to save user profile" << std::endl;
            success = false;
        }

        if (!DailyLog::saveToFile(logFile, dailyLogs))
        {
            std::cerr << "Failed to save daily logs" << std::endl;
            success = false;
        }
    }

    return success;
}

std::string DietManager::getBasicFoodFile() const
{
    return basicFoodFile;
}

std::string DietManager::getCompositeFoodFile() const
{
    return compositeFoodFile;
}

std::string DietManager::getLogFile() const
{
    return logFile;
}

std::string DietManager::getProfileFile() const
{
    return profileFile;
}

std::string DietManager::getUsersFile() const
{
    return usersFile;
}

bool DietManager::isUsernameTaken(const std::string &username) const
{
    // Check if the username exists in the list of all usernames
    std::vector<std::string> usernames = userManager->getAllUsernames();
    return std::find(usernames.begin(), usernames.end(), username) != usernames.end();
}

bool DietManager::createUserWithProfile(const std::string &username, const std::string &password,
                                        const std::string &name, Gender gender, int heightCm)
{
    if (isUsernameTaken(username))
    {
        return false;
    }

    // Create the user account
    if (!registerUser(username, password))
    {
        return false;
    }

    // Login to set up the user's profile
    if (!loginUser(username, password))
    {
        return false;
    }

    // Set up the profile
    updateUserProfile(name, gender, heightCm);

    return true;
}

UserPtr DietManager::getCurrentUser() const
{
    return userManager->getCurrentUser();
}

bool DietManager::saveUserProfile()
{
    if (!isUserLoggedIn() || !userProfile)
    {
        return false;
    }

    return userProfile->saveToFile(profileFile);
}

// --- AddFoodCommand Implementation ---

DietManager::AddFoodCommand::AddFoodCommand(DietManager *manager, const Date &date,
                                            const std::string &foodId, double servings, MealType mealType)
    : manager(manager), date(date), foodId(foodId), servings(servings), mealType(mealType), addedIndex(-1) {}

void DietManager::AddFoodCommand::execute()
{
    FoodPtr food = manager->getFoodDatabase()->getFoodById(foodId);
    if (!food)
    {
        std::cerr << "Error: Food not found: " << foodId << std::endl;
        return;
    }

    DailyLogPtr log = manager->getDailyLog(date);
    if (!log)
    {
        std::cerr << "Error: Could not get daily log" << std::endl;
        return;
    }

    LogEntryPtr entry = std::make_shared<LogEntry>(food, servings, mealType);
    log->addEntry(entry);

    // Store the index for undo
    addedIndex = log->getEntries().size() - 1;
}

void DietManager::AddFoodCommand::undo()
{
    if (addedIndex >= 0)
    {
        DailyLogPtr log = manager->getDailyLog(date);
        if (log)
        {
            log->removeEntry(addedIndex);
        }
    }
}

std::string DietManager::AddFoodCommand::getDescription() const
{
    return "Add food " + foodId + " (" + std::to_string(servings) + " servings) to log for " + date.toString();
}

// --- RemoveFoodCommand Implementation ---

DietManager::RemoveFoodCommand::RemoveFoodCommand(DietManager *manager, const Date &date, int entryIndex)
    : manager(manager), date(date), entryIndex(entryIndex), removedFood(nullptr), removedServings(0) {}

void DietManager::RemoveFoodCommand::execute()
{
    DailyLogPtr log = manager->getDailyLog(date);
    if (!log)
    {
        return;
    }

    std::vector<LogEntryPtr> entries = log->getEntries();

    if (entryIndex >= 0 && entryIndex < static_cast<int>(entries.size()))
    {
        // Store info for undo
        removedFood = entries[entryIndex]->getFood();
        removedServings = entries[entryIndex]->getServings();

        // Remove the entry
        log->removeEntry(entryIndex);
    }
}

void DietManager::RemoveFoodCommand::undo()
{
    if (removedFood)
    {
        DailyLogPtr log = manager->getDailyLog(date);
        if (log)
        {
            LogEntryPtr entry = std::make_shared<LogEntry>(removedFood, removedServings);
            log->addEntry(entry);
        }
    }
}

std::string DietManager::RemoveFoodCommand::getDescription() const
{
    return "Remove food entry at index " + std::to_string(entryIndex) + " from log for " + date.toString();
}

// --- UpdateProfileCommand Implementation ---

DietManager::UpdateProfileCommand::UpdateProfileCommand(DietManager *manager, const std::string &name,
                                                        Gender gender, int heightCm)
    : manager(manager), newName(name), newGender(gender), newHeightCm(heightCm)
{
    // Save original values for undo
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        oldName = profile->getName();
        oldGender = profile->getGender();
        oldHeightCm = profile->getHeightCm();
    }
}

void DietManager::UpdateProfileCommand::execute()
{
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        profile->setName(newName);
        profile->setGender(newGender);
        profile->setHeightCm(newHeightCm);
    }
}

void DietManager::UpdateProfileCommand::undo()
{
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        profile->setName(oldName);
        profile->setGender(oldGender);
        profile->setHeightCm(oldHeightCm);
    }
}

std::string DietManager::UpdateProfileCommand::getDescription() const
{
    return "Update user profile with name=" + newName;
}

// --- SetUserMetricsCommand Implementation ---

DietManager::SetUserMetricsCommand::SetUserMetricsCommand(DietManager *manager, const Date &date,
                                                          int age, double weight, ActivityLevel level)
    : manager(manager), date(date), newAge(age), newWeight(weight), newLevel(level),
      oldAge(0), oldWeight(0), oldLevel(ActivityLevel::ModeratelyActive), hadPreviousMetrics(false)
{

    // Save original values for undo
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        try
        {
            oldAge = profile->getAge(date);
            oldWeight = profile->getWeight(date);
            oldLevel = profile->getActivityLevel(date);
            hadPreviousMetrics = true;
        }
        catch (...)
        {
            hadPreviousMetrics = false;
        }
    }
}

void DietManager::SetUserMetricsCommand::execute()
{
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        profile->setAge(date, newAge);
        profile->setWeight(date, newWeight);
        profile->setActivityLevel(date, newLevel);
    }
}

void DietManager::SetUserMetricsCommand::undo()
{
    if (hadPreviousMetrics)
    {
        UserProfilePtr profile = manager->getUserProfile();
        if (profile)
        {
            profile->setAge(date, oldAge);
            profile->setWeight(date, oldWeight);
            profile->setActivityLevel(date, oldLevel);
        }
    }
}

std::string DietManager::SetUserMetricsCommand::getDescription() const
{
    return "Set user metrics for " + date.toString();
}

// --- SetCalorieMethodCommand Implementation ---

DietManager::SetCalorieMethodCommand::SetCalorieMethodCommand(DietManager *manager, CalorieCalculationMethod method)
    : manager(manager), newMethod(method)
{

    // Save original method for undo
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        oldMethod = profile->getCalorieMethod();
    }
}

void DietManager::SetCalorieMethodCommand::execute()
{
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        profile->setCalorieMethod(newMethod);
    }
}

void DietManager::SetCalorieMethodCommand::undo()
{
    UserProfilePtr profile = manager->getUserProfile();
    if (profile)
    {
        profile->setCalorieMethod(oldMethod);
    }
}

std::string DietManager::SetCalorieMethodCommand::getDescription() const
{
    return "Change calorie calculation method to " + UserProfile::calorieMethodToString(newMethod);
}
