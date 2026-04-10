#include "UserInterface.h"
#include "../model/BasicFood.h"
#include "../model/CompositeFood.h"
#include "../model/DailyLog.h"
#include "../model/Date.h"
#include "../model/User.h"
#include "../controller/CommandManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>

// Define constants used in the UI
const std::string SEPARATOR = std::string(80, '-');
const std::string MAIN_MENU_PROMPT = "Enter command (type 'help' for list of commands): ";

UserInterface::UserInterface(std::shared_ptr<DietManager> dietManager) 
    : dietManager(dietManager), 
      currentDate(Date::today()),
      running(false) {
    // Initialize command handlers
    initializeCommandHandlers();
}

void UserInterface::run() {
    // Display welcome message
    displayWelcome();
    
    // Enter the main command loop
    running = true;
    if (!dietManager->isUserLoggedIn()) {
        handleLogin();
    }
    
    while (running && dietManager->isUserLoggedIn()) {
        std::string command = getInput(MAIN_MENU_PROMPT);
        processCommand(command);
    }
    
    // Display goodbye message
    displayGoodbye();
}

void UserInterface::displayWelcome() {
    std::cout << "\n" << SEPARATOR << std::endl;
    std::cout << "Welcome to YADA (Yet Another Diet Assistant)" << std::endl;
    std::cout << "Version: 1.0" << std::endl;
    std::cout << SEPARATOR << "\n" << std::endl;
}

void UserInterface::displayGoodbye() {
    std::cout << "\n" << SEPARATOR << std::endl;
    std::cout << "Thank you for using YADA!" << std::endl;
    std::cout << "Goodbye!" << std::endl;
    std::cout << SEPARATOR << "\n" << std::endl;
}

void UserInterface::initializeCommandHandlers() {
    // Main commands
    commands["help"] = [this](const std::vector<std::string>& args) { handleHelp(); };
    commands["exit"] = [this](const std::vector<std::string>& args) { handleExit(); };
    commands["logout"] = [this](const std::vector<std::string>& args) { handleLogout(); };
    
    // Food management
    commands["add-food"] = [this](const std::vector<std::string>& args) { handleAddFood(); };
    commands["find-food"] = [this](const std::vector<std::string>& args) { handleFindFood(); };
    commands["view-food"] = [this](const std::vector<std::string>& args) { handleViewFood(args); };
    commands["edit-food"] = [this](const std::vector<std::string>& args) { handleEditFood(args); };
    commands["delete-food"] = [this](const std::vector<std::string>& args) { handleDeleteFood(args); };
    
    // Logging food
    commands["log"] = [this](const std::vector<std::string>& args) { handleLogFood(); };
    commands["today"] = [this](const std::vector<std::string>& args) { handleViewTodayLog(); };
    commands["view-log"] = [this](const std::vector<std::string>& args) { handleViewLog(args); };
    commands["remove-log"] = [this](const std::vector<std::string>& args) { handleRemoveLogEntry(args); };
    
    // Profile management
    commands["profile"] = [this](const std::vector<std::string>& args) { handleViewProfile(); };
    commands["update-profile"] = [this](const std::vector<std::string>& args) { handleUpdateProfile(); };
    commands["set-target"] = [this](const std::vector<std::string>& args) { handleSetTargetCalories(); };
    
    // Date management
    commands["date"] = [this](const std::vector<std::string>& args) { handleSetDate(args); };
    
    // Command history
    commands["undo"] = [this](const std::vector<std::string>& args) { handleUndo(); };
    commands["redo"] = [this](const std::vector<std::string>& args) { handleRedo(); };
}

void UserInterface::processCommand(const std::string& commandLine) {
    // Skip empty commands
    if (commandLine.empty()) {
        return;
    }
    
    // Parse the command and arguments
    std::vector<std::string> parts = parseCommandLine(commandLine);
    
    if (parts.empty()) {
        std::cout << "Invalid command. Type 'help' for available commands." << std::endl;
        return;
    }
    
    // Extract the command and arguments
    std::string command = parts[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    std::vector<std::string> args;
    for (size_t i = 1; i < parts.size(); i++) {
        args.push_back(parts[i]);
    }
    
    // Execute the command
    auto it = commands.find(command);
    if (it != commands.end()) {
        it->second(args);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type 'help' for a list of available commands." << std::endl;
    }
}

std::vector<std::string> UserInterface::parseCommandLine(const std::string& commandLine) {
    std::vector<std::string> parts;
    std::string part;
    bool inQuotes = false;
    
    for (char c : commandLine) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!part.empty()) {
                parts.push_back(part);
                part.clear();
            }
        } else {
            part += c;
        }
    }
    
    if (!part.empty()) {
        parts.push_back(part);
    }
    
    return parts;
}

std::string UserInterface::getInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int UserInterface::getIntInput(const std::string& prompt, int min, int max) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        try {
            int value = std::stoi(input);
            if (value >= min && value <= max) {
                return value;
            }
            std::cout << "Please enter a number between " << min << " and " << max << "." << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
        }
    }
}

double UserInterface::getDoubleInput(const std::string& prompt, double min, double max) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        try {
            double value = std::stod(input);
            if (value >= min && value <= max) {
                return value;
            }
            std::cout << "Please enter a number between " << min << " and " << max << "." << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
        }
    }
}

bool UserInterface::getYesNoInput(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (y/n): ";
        std::string input;
        std::getline(std::cin, input);
        
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        
        if (input == "y" || input == "yes") {
            return true;
        } else if (input == "n" || input == "no") {
            return false;
        }
        
        std::cout << "Please enter 'y' or 'n'." << std::endl;
    }
}

void UserInterface::handleHelp() {
    std::cout << "\nAvailable Commands:\n" << SEPARATOR << std::endl;
    
    // Authentication commands
    std::cout << "Authentication:" << std::endl;
    std::cout << "  login             - Log in to the system" << std::endl;
    std::cout << "  logout            - Log out from the system" << std::endl;
    std::cout << "  exit              - Exit the application" << std::endl;
    
    // Food database commands
    std::cout << "\nFood Database:" << std::endl;
    std::cout << "  add-food          - Add a new food to the database" << std::endl;
    std::cout << "  find-food         - Search for food in the database" << std::endl;
    std::cout << "  view-food <id>    - View details of a specific food" << std::endl;
    std::cout << "  edit-food <id>    - Edit a food in the database" << std::endl;
    std::cout << "  delete-food <id>  - Delete a food from the database" << std::endl;
    
    // Food logging commands
    std::cout << "\nFood Logging:" << std::endl;
    std::cout << "  log               - Log food consumption" << std::endl;
    std::cout << "  today             - View today's food log" << std::endl;
    std::cout << "  view-log [date]   - View food log for a specific date" << std::endl;
    std::cout << "  remove-log <id>   - Remove an entry from the log" << std::endl;
    
    // Profile commands
    std::cout << "\nUser Profile:" << std::endl;
    std::cout << "  profile           - View your profile" << std::endl;
    std::cout << "  update-profile    - Update your profile information" << std::endl;
    std::cout << "  set-target        - Set your target calorie intake" << std::endl;
    
    // Date commands
    std::cout << "\nDate Management:" << std::endl;
    std::cout << "  date [YYYY-MM-DD] - Change the current working date" << std::endl;
    
    // Command history
    std::cout << "\nCommand History:" << std::endl;
    std::cout << "  undo              - Undo the last command" << std::endl;
    std::cout << "  redo              - Redo the last undone command" << std::endl;
    
    std::cout << SEPARATOR << std::endl;
}

void UserInterface::handleExit() {
    running = false;
}

void UserInterface::handleLogin() {
    std::cout << "\nLogin to YADA\n" << SEPARATOR << std::endl;
    
    bool loginSuccess = false;
    bool createNewUser = false;
    
    std::cout << "Do you have an account?" << std::endl;
    bool hasAccount = getYesNoInput("Login to existing account?");
    
    if (hasAccount) {
        // Login to existing account
        int attempts = 0;
        const int MAX_ATTEMPTS = 3;
        
        while (attempts < MAX_ATTEMPTS && !loginSuccess) {
            std::string username = getInput("Username: ");
            std::string password = getInput("Password: ");
            
            loginSuccess = dietManager->loginUser(username, password);
            
            if (!loginSuccess) {
                std::cout << "Invalid username or password." << std::endl;
                
                if (++attempts < MAX_ATTEMPTS) {
                    std::cout << "Please try again (" << MAX_ATTEMPTS - attempts << " attempts remaining)." << std::endl;
                } else {
                    std::cout << "Maximum login attempts reached." << std::endl;
                    createNewUser = getYesNoInput("Would you like to create a new account?");
                }
            }
        }
    } else {
        createNewUser = true;
    }
    
    if (createNewUser) {
        // Create a new account
        std::cout << "\nCreate a New Account\n" << SEPARATOR << std::endl;
        
        std::string username;
        bool usernameTaken = false;
        
        do {
            username = getInput("Choose a username: ");
            
            if (username.empty()) {
                std::cout << "Username cannot be empty." << std::endl;
                continue;
            }
            
            usernameTaken = dietManager->isUsernameTaken(username);
            
            if (usernameTaken) {
                std::cout << "This username is already taken. Please choose another one." << std::endl;
            }
        } while (username.empty() || usernameTaken);
        
        std::string password;
        std::string confirmPassword;
        
        do {
            password = getInput("Choose a password: ");
            
            if (password.empty()) {
                std::cout << "Password cannot be empty." << std::endl;
                continue;
            }
            
            confirmPassword = getInput("Confirm password: ");
            
            if (password != confirmPassword) {
                std::cout << "Passwords do not match. Please try again." << std::endl;
            }
        } while (password.empty() || password != confirmPassword);
        
        // Get basic profile information
        std::string name = getInput("Enter your name: ");
        
        // Gender selection
        std::cout << "Gender:" << std::endl;
        std::cout << "1. Male" << std::endl;
        std::cout << "2. Female" << std::endl;
        int genderChoice = getIntInput("Choose (1-2): ", 1, 2);
        Gender gender = (genderChoice == 1) ? Gender::Male : Gender::Female;
        
        // Height
        int heightCm = getIntInput("Enter your height in cm: ", 50, 250);
        
        // Create the account
        loginSuccess = dietManager->createUserWithProfile(username, password, name, gender, heightCm);
        
        if (loginSuccess) {
            std::cout << "\nAccount created successfully!" << std::endl;
        } else {
            std::cout << "\nFailed to create account." << std::endl;
        }
    }
    
    if (loginSuccess) {
        std::cout << "\nWelcome, " << dietManager->getCurrentUser()->getUsername() << "!" << std::endl;
    } else {
        // If login failed and user doesn't want to create an account, exit
        running = false;
    }
}

void UserInterface::handleLogout() {
    dietManager->logoutUser();
    std::cout << "You have been logged out." << std::endl;
    handleLogin();
}

void UserInterface::handleAddFood() {
    std::cout << "\nAdd Food to Database\n" << SEPARATOR << std::endl;
    
    std::cout << "1. Add Basic Food" << std::endl;
    std::cout << "2. Add Recipe (Composite Food)" << std::endl;
    
    int choice = getIntInput("Choose an option (1-2): ", 1, 2);
    
    if (choice == 1) {
        addBasicFood();
    } else {
        addCompositeFood();
    }
}

void UserInterface::addBasicFood() {
    std::cout << "\nAdd Basic Food\n" << SEPARATOR << std::endl;
    
    // Get the food ID
    std::string id;
    bool idExists = false;
    
    do {
        id = getInput("Enter a unique ID for the food: ");
        
        // Remove spaces and make lowercase
        id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
        std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        
        if (id.empty()) {
            std::cout << "ID cannot be empty." << std::endl;
            continue;
        }
        
        // Check if the ID already exists
        idExists = dietManager->getFoodDatabase()->getFoodById(id) != nullptr;
        
        if (idExists) {
            std::cout << "This ID already exists. Please choose another one." << std::endl;
        }
    } while (id.empty() || idExists);
    
    // Get keywords (comma-separated)
    std::string keywordsString = getInput("Enter keywords (comma-separated): ");
    
    // Parse the keywords
    std::vector<std::string> keywords = parseKeywords(keywordsString);
    
    // Get calories per serving
    double calories = getDoubleInput("Enter calories per serving: ", 0, 10000);
    
    // Create the food and add it to the database
    auto basicFood = std::make_shared<BasicFood>(id, keywords, calories);
    bool success = dietManager->getFoodDatabase()->addFood(basicFood);
    
    if (success) {
        std::cout << "Basic food added successfully!" << std::endl;
    } else {
        std::cout << "Failed to add food." << std::endl;
    }
}

void UserInterface::addCompositeFood() {
    std::cout << "\nAdd Recipe (Composite Food)\n" << SEPARATOR << std::endl;
    
    // Get the food ID
    std::string id;
    bool idExists = false;
    
    do {
        id = getInput("Enter a unique ID for the recipe: ");
        
        // Remove spaces and make lowercase
        id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
        std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        
        if (id.empty()) {
            std::cout << "ID cannot be empty." << std::endl;
            continue;
        }
        
        // Check if the ID already exists
        idExists = dietManager->getFoodDatabase()->getFoodById(id) != nullptr;
        
        if (idExists) {
            std::cout << "This ID already exists. Please choose another one." << std::endl;
        }
    } while (id.empty() || idExists);
    
    // Get keywords (comma-separated)
    std::string keywordsString = getInput("Enter keywords (comma-separated): ");
    
    // Parse the keywords
    std::vector<std::string> keywords = parseKeywords(keywordsString);
    
    // Create the composite food
    auto compositeFood = std::make_shared<CompositeFood>(id, keywords);
    
    // Add ingredients
    bool addMoreIngredients = true;
    
    std::cout << "\nAdd ingredients to the recipe:" << std::endl;
    
    while (addMoreIngredients) {
        // Search for an ingredient
        std::cout << "\nSearch for an ingredient:" << std::endl;
        std::string searchQuery = getInput("Enter search term: ");
        
        std::vector<FoodPtr> searchResults = searchFoodsHelper(searchQuery);
        
        if (searchResults.empty()) {
            std::cout << "No foods found matching '" << searchQuery << "'." << std::endl;
        } else {
            std::cout << "\nSearch Results:" << std::endl;
            displayFoodList(searchResults);
            
            // Select an ingredient
            std::string ingredientId = getInput("\nEnter the ID of the ingredient to add (or leave empty to cancel): ");
            
            if (!ingredientId.empty()) {
                // Find the selected food
                FoodPtr ingredient = dietManager->getFoodDatabase()->getFoodById(ingredientId);
                
                if (!ingredient) {
                    std::cout << "Invalid ingredient ID." << std::endl;
                } else {
                    // Check if this would create a circular reference
                    if (ingredient->getId() == id) {
                        std::cout << "Error: Cannot add a recipe as an ingredient of itself." << std::endl;
                    } else if (ingredient->isComposite() && isCircularReference(std::dynamic_pointer_cast<CompositeFood>(ingredient), id)) {
                        std::cout << "Error: This would create a circular reference in the recipe." << std::endl;
                    } else {
                        // Get the number of servings
                        double servings = getDoubleInput("Enter the number of servings: ", 0.01, 1000);
                        
                        // Add the ingredient
                        compositeFood->addComponent(ingredient, servings);
                        std::cout << "Added " << servings << " serving(s) of " << ingredient->getId() << " to the recipe." << std::endl;
                    }
                }
            }
        }
        
        // Ask if the user wants to add more ingredients
        addMoreIngredients = getYesNoInput("\nAdd another ingredient?");
    }
    
    // Check if the recipe has at least one ingredient
    if (compositeFood->getComponents().empty()) {
        std::cout << "The recipe must have at least one ingredient." << std::endl;
        return;
    }
    
    // Add the composite food to the database
    bool success = dietManager->getFoodDatabase()->addFood(compositeFood);
    
    if (success) {
        std::cout << "Recipe added successfully!" << std::endl;
    } else {
        std::cout << "Failed to add recipe." << std::endl;
    }
}

bool UserInterface::isCircularReference(const std::shared_ptr<CompositeFood>& food, const std::string& targetId) {
    for (const auto& component : food->getComponents()) {
        if (component.first->getId() == targetId) {
            return true;
        }
        
        if (component.first->isComposite()) {
            if (isCircularReference(std::dynamic_pointer_cast<CompositeFood>(component.first), targetId)) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::string> UserInterface::parseKeywords(const std::string& keywordsString) {
    std::vector<std::string> keywords;
    std::stringstream ss(keywordsString);
    std::string keyword;
    
    while (getline(ss, keyword, ',')) {
        // Trim whitespace
        keyword.erase(0, keyword.find_first_not_of(" \t\n\r\f\v"));
        keyword.erase(keyword.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (!keyword.empty()) {
            // Convert to lowercase
            std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
            keywords.push_back(keyword);
        }
    }
    
    return keywords;
}

void UserInterface::handleFindFood() {
    std::cout << "\nSearch Food Database\n" << SEPARATOR << std::endl;
    
    std::string searchQuery = getInput("Enter search term: ");
    
    if (searchQuery.empty()) {
        std::cout << "Search term cannot be empty." << std::endl;
        return;
    }
    
    std::vector<FoodPtr> searchResults = searchFoodsHelper(searchQuery);
    
    if (searchResults.empty()) {
        std::cout << "No foods found matching '" << searchQuery << "'." << std::endl;
    } else {
        std::cout << "\nSearch Results:" << std::endl;
        displayFoodList(searchResults);
    }
}

void UserInterface::handleViewFood(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: view-food <food-id>" << std::endl;
        return;
    }
    
    std::string foodId = args[0];
    FoodPtr food = dietManager->getFoodDatabase()->getFoodById(foodId);
    
    if (!food) {
        std::cout << "Food not found: " << foodId << std::endl;
        return;
    }
    
    std::cout << "\nFood Details\n" << SEPARATOR << std::endl;
    std::cout << "ID: " << food->getId() << std::endl;
    std::cout << "Type: " << (food->isComposite() ? "Recipe (Composite)" : "Basic Food") << std::endl;
    std::cout << "Calories: " << food->getCaloriesPerServing() << " per serving" << std::endl;
    
    std::cout << "Keywords: ";
    const std::vector<std::string>& keywords = food->getKeywords();
    for (size_t i = 0; i < keywords.size(); i++) {
        std::cout << keywords[i];
        if (i < keywords.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    if (food->isComposite()) {
        std::cout << "\nIngredients:" << std::endl;
        std::cout << std::setw(20) << std::left << "Food ID";
        std::cout << std::setw(10) << std::right << "Servings";
        std::cout << std::setw(15) << std::right << "Calories";
        std::cout << std::endl;
        std::cout << std::string(45, '-') << std::endl;
        
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(food);
        const auto& components = compositeFood->getComponents();
        
        for (const auto& component : components) {
            std::cout << std::setw(20) << std::left << component.first->getId();
            std::cout << std::setw(10) << std::right << std::fixed << std::setprecision(2) << component.second;
            
            double calories = component.first->getCaloriesPerServing() * component.second;
            std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << calories;
            
            std::cout << std::endl;
        }
        
        std::cout << std::string(45, '-') << std::endl;
        std::cout << std::setw(30) << std::left << "Total Calories:";
        std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) 
                  << food->getCaloriesPerServing() << std::endl;
    }
}

void UserInterface::handleEditFood(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: edit-food <food-id>" << std::endl;
        return;
    }
    
    std::string foodId = args[0];
    FoodPtr food = dietManager->getFoodDatabase()->getFoodById(foodId);
    
    if (!food) {
        std::cout << "Food not found: " << foodId << std::endl;
        return;
    }
    
    std::cout << "\nEdit Food\n" << SEPARATOR << std::endl;
    std::cout << "Current Food Details:" << std::endl;
    std::cout << "ID: " << food->getId() << std::endl;
    std::cout << "Type: " << (food->isComposite() ? "Recipe (Composite)" : "Basic Food") << std::endl;
    
    // Edit keywords
    std::cout << "\nCurrent keywords: ";
    const std::vector<std::string>& keywords = food->getKeywords();
    for (size_t i = 0; i < keywords.size(); i++) {
        std::cout << keywords[i];
        if (i < keywords.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    if (getYesNoInput("Edit keywords?")) {
        std::string keywordsString = getInput("Enter new keywords (comma-separated): ");
        std::vector<std::string> newKeywords = parseKeywords(keywordsString);
        
        // Remove all current keywords and add the new ones
        if (!newKeywords.empty()) {
            for (const auto& keyword : keywords) {
                // We'd need to implement removeKeyword in the Food class
                // For now, just rebuild the list
            }
            
            for (const auto& keyword : newKeywords) {
                food->addKeyword(keyword);
            }
        }
    }
    
    if (!food->isComposite()) {
        // Edit calories per serving for basic food
        auto basicFood = std::dynamic_pointer_cast<BasicFood>(food);
        
        std::cout << "Current calories per serving: " << basicFood->getCaloriesPerServing() << std::endl;
        
        if (getYesNoInput("Edit calories per serving?")) {
            double newCalories = getDoubleInput("Enter new calories per serving: ", 0, 10000);
            
            // We'd need to implement setCaloriesPerServing in the BasicFood class
            // For now, let's create a new BasicFood and replace it in the database
            auto updatedFood = std::make_shared<BasicFood>(basicFood->getId(), basicFood->getKeywords(), newCalories);
            
            if (dietManager->getFoodDatabase()->updateFood(updatedFood)) {
                std::cout << "Food updated successfully!" << std::endl;
            } else {
                std::cout << "Failed to update food." << std::endl;
            }
        }
    } else {
        // Edit components for composite food
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(food);
        
        std::cout << "\nCurrent ingredients:" << std::endl;
        const auto& components = compositeFood->getComponents();
        for (const auto& component : components) {
            std::cout << "- " << component.first->getId() << " (" << component.second << " servings)" << std::endl;
        }
        
        if (getYesNoInput("Edit ingredients?")) {
            // For now, we'll create a new composite food and replace it
            auto updatedFood = std::make_shared<CompositeFood>(compositeFood->getId(), compositeFood->getKeywords());
            
            // Add ingredients
            bool addMoreIngredients = true;
            
            while (addMoreIngredients) {
                // Search for an ingredient
                std::cout << "\nSearch for an ingredient:" << std::endl;
                std::string searchQuery = getInput("Enter search term: ");
                
                std::vector<FoodPtr> searchResults = searchFoodsHelper(searchQuery);
                
                if (searchResults.empty()) {
                    std::cout << "No foods found matching '" << searchQuery << "'." << std::endl;
                } else {
                    std::cout << "\nSearch Results:" << std::endl;
                    displayFoodList(searchResults);
                    
                    // Select an ingredient
                    std::string ingredientId = getInput("\nEnter the ID of the ingredient to add (or leave empty to cancel): ");
                    
                    if (!ingredientId.empty()) {
                        // Find the selected food
                        FoodPtr ingredient = dietManager->getFoodDatabase()->getFoodById(ingredientId);
                        
                        if (!ingredient) {
                            std::cout << "Invalid ingredient ID." << std::endl;
                        } else {
                            // Check if this would create a circular reference
                            if (ingredient->getId() == updatedFood->getId()) {
                                std::cout << "Error: Cannot add a recipe as an ingredient of itself." << std::endl;
                            } else if (ingredient->isComposite() && 
                                      isCircularReference(std::dynamic_pointer_cast<CompositeFood>(ingredient), updatedFood->getId())) {
                                std::cout << "Error: This would create a circular reference in the recipe." << std::endl;
                            } else {
                                // Get the number of servings
                                double servings = getDoubleInput("Enter the number of servings: ", 0.01, 1000);
                                
                                // Add the ingredient
                                updatedFood->addComponent(ingredient, servings);
                                std::cout << "Added " << servings << " serving(s) of " << ingredient->getId() << " to the recipe." << std::endl;
                            }
                        }
                    }
                }
                
                // Ask if the user wants to add more ingredients
                addMoreIngredients = getYesNoInput("\nAdd another ingredient?");
            }
            
            // Check if the recipe has at least one ingredient
            if (updatedFood->getComponents().empty()) {
                std::cout << "The recipe must have at least one ingredient." << std::endl;
                return;
            }
            
            // Update the composite food in the database
            if (dietManager->getFoodDatabase()->updateFood(updatedFood)) {
                std::cout << "Recipe updated successfully!" << std::endl;
            } else {
                std::cout << "Failed to update recipe." << std::endl;
            }
        }
    }
}

void UserInterface::handleDeleteFood(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: delete-food <food-id>" << std::endl;
        return;
    }
    
    std::string foodId = args[0];
    FoodPtr food = dietManager->getFoodDatabase()->getFoodById(foodId);
    
    if (!food) {
        std::cout << "Food not found: " << foodId << std::endl;
        return;
    }
    
    std::cout << "\nDelete Food\n" << SEPARATOR << std::endl;
    std::cout << "You are about to delete the following food:" << std::endl;
    std::cout << "ID: " << food->getId() << std::endl;
    std::cout << "Type: " << (food->isComposite() ? "Recipe (Composite)" : "Basic Food") << std::endl;
    std::cout << "Calories: " << food->getCaloriesPerServing() << " per serving" << std::endl;
    
    // Check if this food is used in any recipes
    bool isUsedInRecipes = false;
    std::vector<std::string> usedInRecipes;
    
    auto allFoods = dietManager->getFoodDatabase()->getAllFoods();
    for (const auto& otherFood : allFoods) {
        if (otherFood->isComposite()) {
            auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(otherFood);
            const auto& components = compositeFood->getComponents();
            
            for (const auto& component : components) {
                if (component.first->getId() == foodId) {
                    isUsedInRecipes = true;
                    usedInRecipes.push_back(otherFood->getId());
                    break;
                }
            }
        }
    }
    
    if (isUsedInRecipes) {
        std::cout << "\nWarning: This food is used in the following recipes:" << std::endl;
        for (const auto& recipeId : usedInRecipes) {
            std::cout << "- " << recipeId << std::endl;
        }
        std::cout << "Deleting this food will remove it from these recipes." << std::endl;
    }
    
    // Confirm deletion
    if (getYesNoInput("\nAre you sure you want to delete this food?")) {
        if (dietManager->getFoodDatabase()->removeFood(foodId)) {
            std::cout << "Food deleted successfully!" << std::endl;
        } else {
            std::cout << "Failed to delete food." << std::endl;
        }
    } else {
        std::cout << "Deletion cancelled." << std::endl;
    }
}

void UserInterface::displayFoodList(const std::vector<FoodPtr>& foods) {
    if (foods.empty()) {
        std::cout << "No foods found." << std::endl;
        return;
    }
    
    std::cout << std::setw(15) << std::left << "ID";
    std::cout << std::setw(10) << std::left << "Type";
    std::cout << std::setw(15) << std::right << "Calories";
    std::cout << "  Keywords" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& food : foods) {
        std::cout << std::setw(15) << std::left << food->getId();
        std::cout << std::setw(10) << std::left << (food->isComposite() ? "Composite" : "Basic");
        std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << food->getCaloriesPerServing();
        
        // Print keywords
        std::cout << "  ";
        const std::vector<std::string>& keywords = food->getKeywords();
        for (size_t i = 0; i < keywords.size(); i++) {
            std::cout << keywords[i];
            if (i < keywords.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}

void UserInterface::handleLogFood() {
    std::cout << "\nLog Food Consumption\n" << SEPARATOR << std::endl;
    
    // Display current date
    std::cout << "Current date: " << currentDate.toString() << std::endl;
    
    // Search for food to log
    std::string searchQuery = getInput("Search for food to log: ");
    
    if (searchQuery.empty()) {
        std::cout << "Search term cannot be empty." << std::endl;
        return;
    }
    
    std::vector<FoodPtr> searchResults = searchFoodsHelper(searchQuery);
    
    if (searchResults.empty()) {
        std::cout << "No foods found matching '" << searchQuery << "'." << std::endl;
        return;
    }
    
    std::cout << "\nSearch Results:" << std::endl;
    displayFoodList(searchResults);
    
    // Select a food to log
    std::string foodId = getInput("\nEnter the ID of the food to log (or leave empty to cancel): ");
    
    if (foodId.empty()) {
        std::cout << "Logging cancelled." << std::endl;
        return;
    }
    
    FoodPtr food = dietManager->getFoodDatabase()->getFoodById(foodId);
    
    if (!food) {
        std::cout << "Invalid food ID." << std::endl;
        return;
    }
    
    // Get the number of servings
    double servings = getDoubleInput("Enter the number of servings: ", 0.01, 1000);
    
    // Get the meal type
    std::cout << "\nMeal type:" << std::endl;
    std::cout << "1. Breakfast" << std::endl;
    std::cout << "2. Lunch" << std::endl;
    std::cout << "3. Dinner" << std::endl;
    std::cout << "4. Snack" << std::endl;
    
    int mealTypeChoice = getIntInput("Choose a meal type (1-4): ", 1, 4);
    std::string mealType;
    
    switch (mealTypeChoice) {
        case 1: mealType = "Breakfast"; break;
        case 2: mealType = "Lunch"; break;
        case 3: mealType = "Dinner"; break;
        case 4: mealType = "Snack"; break;
        default: mealType = "Other"; break;
    }
    
    // Add the food to the daily log
    if (dietManager->addFoodToLog(currentDate, food->getId(), servings, stringToMealType(mealType))) {
        std::cout << "Food logged successfully!" << std::endl;
    } else {
        std::cout << "Failed to log food." << std::endl;
    }
}

void UserInterface::handleViewTodayLog() {
    std::cout << "\nToday's Food Log\n" << SEPARATOR << std::endl;
    
    // Display current date
    std::cout << "Current date: " << currentDate.toString() << std::endl;
    
    // Get the daily log
    auto dailyLog = dietManager->getDailyLog(currentDate);
    
    if (!dailyLog || dailyLog->getEntries().empty()) {
        std::cout << "No food logged for today." << std::endl;
        
        // Show target calories
        auto userProfile = dietManager->getUserProfile();
        if (userProfile) {
            double targetCalories = userProfile->getTargetCalories(currentDate);
            std::cout << "\nTarget calories for today: " << std::fixed << std::setprecision(1) << targetCalories << std::endl;
        }
        
        return;
    }
    
    displayDailyLog(dailyLog, currentDate);
}

void UserInterface::handleViewLog(const std::vector<std::string>& args) {
    std::cout << "\nView Food Log\n" << SEPARATOR << std::endl;
    
    Date date;
    
    if (args.empty()) {
        // Ask for a date
        date = getDateInput("Enter date (YYYY-MM-DD): ");
    } else {
        // Parse the date from the argument
        try {
            date = Date(args[0]);
        } catch (const std::exception& e) {
            std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
            return;
        }
    }
    
    // Get the daily log
    auto dailyLog = dietManager->getDailyLog(date);
    
    if (!dailyLog || dailyLog->getEntries().empty()) {
        std::cout << "No food logged for " << date.toString() << "." << std::endl;
        
        // Show target calories
        auto userProfile = dietManager->getUserProfile();
        if (userProfile) {
            double targetCalories = userProfile->getTargetCalories(date);
            std::cout << "\nTarget calories for this date: " << std::fixed << std::setprecision(1) << targetCalories << std::endl;
        }
        
        return;
    }
    
    displayDailyLog(dailyLog, date);
}

void UserInterface::displayDailyLog(const std::shared_ptr<DailyLog>& dailyLog, const Date& date) {
    // Display the entries grouped by meal type
    std::map<std::string, std::vector<LogEntryPtr>> entriesByMealType;
    
    // Group entries by meal type
    for (const auto& entry : dailyLog->getEntries()) {
        entriesByMealType[entry->getMealTypeString()].push_back(entry);
    }
    
    // Display entries for each meal type
    for (const auto& pair : entriesByMealType) {
        std::cout << "\n" << pair.first << ":" << std::endl;
        std::cout << std::setw(30) << std::left << "Food";
        std::cout << std::setw(10) << std::right << "Servings";
        std::cout << std::setw(15) << std::right << "Calories";
        std::cout << std::endl;
        std::cout << std::string(55, '-') << std::endl;
        
        double mealTypeCalories = 0.0;
        
        for (const auto& entry : pair.second) {
            FoodPtr food = entry->getFood();
            double servings = entry->getServings();
            double calories = food->getCaloriesPerServing() * servings;
            
            std::cout << std::setw(30) << std::left << food->getId();
            std::cout << std::setw(10) << std::right << std::fixed << std::setprecision(2) << servings;
            std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << calories;
            std::cout << std::endl;
            
            mealTypeCalories += calories;
        }
        
        std::cout << std::string(55, '-') << std::endl;
        std::cout << std::setw(40) << std::left << "Total for " + pair.first + ":";
        std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << mealTypeCalories << std::endl;
    }
    
    // Display total calories for the day
    double totalCalories = dailyLog->getTotalCalories();
    std::cout << "\n" << std::string(55, '=') << std::endl;
    std::cout << std::setw(40) << std::left << "Total Calories for the Day:";
    std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << totalCalories << std::endl;
    
    // Display target calories and the difference
    auto userProfile = dietManager->getUserProfile();
    if (userProfile) {
        double targetCalories = userProfile->getTargetCalories(date);
        double difference = targetCalories - totalCalories;
        
        std::cout << std::setw(40) << std::left << "Target Calories:";
        std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << targetCalories << std::endl;
        
        std::cout << std::setw(40) << std::left << "Remaining Calories:";
        std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << difference << std::endl;
    }
}

void UserInterface::handleRemoveLogEntry(const std::vector<std::string>& args) {
    std::cout << "\nRemove Log Entry\n" << SEPARATOR << std::endl;
    
    // Display current date
    std::cout << "Current date: " << currentDate.toString() << std::endl;
    
    // Get the daily log
    auto dailyLog = dietManager->getDailyLog(currentDate);
    
    if (!dailyLog || dailyLog->getEntries().empty()) {
        std::cout << "No food logged for today." << std::endl;
        return;
    }
    
    // Display the entries with indices
    const auto& entries = dailyLog->getEntries();
    std::cout << "\nToday's Log Entries:" << std::endl;
    std::cout << std::setw(5) << std::left << "Index";
    std::cout << std::setw(15) << std::left << "Food ID";
    std::cout << std::setw(10) << std::left << "Meal Type";
    std::cout << std::setw(10) << std::right << "Servings";
    std::cout << std::setw(15) << std::right << "Calories";
    std::cout << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    for (size_t i = 0; i < entries.size(); i++) {
        const auto& entry = entries[i];
        FoodPtr food = entry->getFood();
        double servings = entry->getServings();
        double calories = food->getCaloriesPerServing() * servings;
        
        std::cout << std::setw(5) << std::left << (i + 1);
        std::cout << std::setw(15) << std::left << food->getId();
        std::cout << std::setw(10) << std::left << entry->getMealTypeString();
        std::cout << std::setw(10) << std::right << std::fixed << std::setprecision(2) << servings;
        std::cout << std::setw(15) << std::right << std::fixed << std::setprecision(1) << calories;
        std::cout << std::endl;
    }
    
    // Ask for the index to remove
    int indexToRemove = getIntInput("\nEnter the index of the entry to remove (or 0 to cancel): ", 0, entries.size());
    
    if (indexToRemove == 0) {
        std::cout << "Removal cancelled." << std::endl;
        return;
    }
    
    // Remove the entry
    if (dietManager->removeFoodFromLog(currentDate, indexToRemove - 1)) {
        std::cout << "Log entry removed successfully!" << std::endl;
    } else {
        std::cout << "Failed to remove log entry." << std::endl;
    }
}

void UserInterface::handleViewProfile() {
    std::cout << "\nUser Profile\n" << SEPARATOR << std::endl;
    
    auto userProfile = dietManager->getUserProfile();
    if (!userProfile) {
        std::cout << "No user profile available." << std::endl;
        return;
    }
    
    std::cout << "Name: " << userProfile->getName() << std::endl;
    std::cout << "Gender: " << UserProfile::genderToString(userProfile->getGender()) << std::endl;
    std::cout << "Height: " << userProfile->getHeightCm() << " cm" << std::endl;
    
    // Get the metrics for the current date
    int age = userProfile->getAge(currentDate);
    double weight = userProfile->getWeight(currentDate);
    ActivityLevel activityLevel = userProfile->getActivityLevel(currentDate);
    
    std::cout << "\nCurrent Metrics (as of " << currentDate.toString() << "):" << std::endl;
    std::cout << "Age: " << age << " years" << std::endl;
    std::cout << "Weight: " << weight << " kg" << std::endl;
    std::cout << "Activity Level: " << UserProfile::activityLevelToString(activityLevel) << std::endl;
    
    // Display calorie calculation method
    CalorieCalculationMethod method = userProfile->getCalorieMethod();
    std::cout << "\nCalorie Calculation Method: " << UserProfile::calorieMethodToString(method) << std::endl;
    
    if (method == CalorieCalculationMethod::FixedGoal) {
        std::cout << "Fixed Daily Calorie Goal: " << userProfile->getFixedCalorieGoal() << std::endl;
    }
    
    // Display target calories
    double targetCalories = userProfile->getTargetCalories(currentDate);
    std::cout << "Target Daily Calories: " << std::fixed << std::setprecision(1) << targetCalories << std::endl;
}

void UserInterface::handleUpdateProfile() {
    std::cout << "\nUpdate Profile\n" << SEPARATOR << std::endl;
    
    auto userProfile = dietManager->getUserProfile();
    if (!userProfile) {
        std::cout << "No user profile available." << std::endl;
        return;
    }
    
    std::cout << "Current Profile:" << std::endl;
    std::cout << "Name: " << userProfile->getName() << std::endl;
    std::cout << "Gender: " << UserProfile::genderToString(userProfile->getGender()) << std::endl;
    std::cout << "Height: " << userProfile->getHeightCm() << " cm" << std::endl;
    
    // Get the metrics for the current date
    int age = userProfile->getAge(currentDate);
    double weight = userProfile->getWeight(currentDate);
    ActivityLevel activityLevel = userProfile->getActivityLevel(currentDate);
    
    std::cout << "\nCurrent Metrics (as of " << currentDate.toString() << "):" << std::endl;
    std::cout << "Age: " << age << " years" << std::endl;
    std::cout << "Weight: " << weight << " kg" << std::endl;
    std::cout << "Activity Level: " << UserProfile::activityLevelToString(activityLevel) << std::endl;
    
    // Update basic profile information
    if (getYesNoInput("\nUpdate basic profile information?")) {
        std::string name = getInput("Enter name (or leave empty to keep current): ");
        if (!name.empty()) {
            userProfile->setName(name);
        }
        
        std::cout << "Current gender: " << UserProfile::genderToString(userProfile->getGender()) << std::endl;
        if (getYesNoInput("Update gender?")) {
            std::cout << "Gender:" << std::endl;
            std::cout << "1. Male" << std::endl;
            std::cout << "2. Female" << std::endl;
            int genderChoice = getIntInput("Choose (1-2): ", 1, 2);
            Gender gender = (genderChoice == 1) ? Gender::Male : Gender::Female;
            userProfile->setGender(gender);
        }
        
        std::cout << "Current height: " << userProfile->getHeightCm() << " cm" << std::endl;
        if (getYesNoInput("Update height?")) {
            int heightCm = getIntInput("Enter height in cm: ", 50, 250);
            userProfile->setHeightCm(heightCm);
        }
    }
    
    // Update current metrics
    if (getYesNoInput("\nUpdate current metrics?")) {
        std::cout << "Current age: " << age << " years" << std::endl;
        if (getYesNoInput("Update age?")) {
            int newAge = getIntInput("Enter age in years: ", 1, 120);
            userProfile->setAge(currentDate, newAge);
        }
        
        std::cout << "Current weight: " << weight << " kg" << std::endl;
        if (getYesNoInput("Update weight?")) {
            double newWeight = getDoubleInput("Enter weight in kg: ", 20, 500);
            userProfile->setWeight(currentDate, newWeight);
        }
        
        std::cout << "Current activity level: " << UserProfile::activityLevelToString(activityLevel) << std::endl;
        if (getYesNoInput("Update activity level?")) {
            std::cout << "Activity Level:" << std::endl;
            std::cout << "1. Sedentary (little or no exercise)" << std::endl;
            std::cout << "2. Lightly Active (light exercise/sports 1-3 days/week)" << std::endl;
            std::cout << "3. Moderately Active (moderate exercise/sports 3-5 days/week)" << std::endl;
            std::cout << "4. Very Active (hard exercise/sports 6-7 days a week)" << std::endl;
            std::cout << "5. Extremely Active (very hard exercise/physical job/training twice a day)" << std::endl;
            
            int activityChoice = getIntInput("Choose (1-5): ", 1, 5);
            ActivityLevel newLevel;
            
            switch (activityChoice) {
                case 1: newLevel = ActivityLevel::Sedentary; break;
                case 2: newLevel = ActivityLevel::LightlyActive; break;
                case 3: newLevel = ActivityLevel::ModeratelyActive; break;
                case 4: newLevel = ActivityLevel::VeryActive; break;
                case 5: newLevel = ActivityLevel::ExtremelyActive; break;
                default: newLevel = ActivityLevel::ModeratelyActive; break;
            }
            
            userProfile->setActivityLevel(currentDate, newLevel);
        }
    }
    
    // Save the profile changes
    if (dietManager->saveUserProfile()) {
        std::cout << "\nProfile updated successfully!" << std::endl;
    } else {
        std::cout << "\nFailed to save profile changes." << std::endl;
    }
}

void UserInterface::handleSetTargetCalories() {
    std::cout << "\nSet Target Calorie Method\n" << SEPARATOR << std::endl;
    
    auto userProfile = dietManager->getUserProfile();
    if (!userProfile) {
        std::cout << "No user profile available." << std::endl;
        return;
    }
    
    // Display current method
    CalorieCalculationMethod currentMethod = userProfile->getCalorieMethod();
    std::cout << "Current method: " << UserProfile::calorieMethodToString(currentMethod) << std::endl;
    
    // Display current target calories
    double targetCalories = userProfile->getTargetCalories(currentDate);
    std::cout << "Current target calories: " << std::fixed << std::setprecision(1) << targetCalories << std::endl;
    
    // Choose a new method
    std::cout << "\nAvailable Methods:" << std::endl;
    std::cout << "1. Harris-Benedict Equation" << std::endl;
    std::cout << "2. Mifflin-St Jeor Equation" << std::endl;
    std::cout << "3. Fixed Calorie Goal" << std::endl;
    
    int methodChoice = getIntInput("Choose a method (1-3): ", 1, 3);
    CalorieCalculationMethod newMethod;
    
    switch (methodChoice) {
        case 1: newMethod = CalorieCalculationMethod::Harris_Benedict; break;
        case 2: newMethod = CalorieCalculationMethod::Mifflin_St_Jeor; break;
        case 3: newMethod = CalorieCalculationMethod::FixedGoal; break;
        default: newMethod = CalorieCalculationMethod::Harris_Benedict; break;
    }
    
    userProfile->setCalorieMethod(newMethod);
    
    // If fixed goal method is chosen, ask for the goal
    if (newMethod == CalorieCalculationMethod::FixedGoal) {
        double fixedGoal = getDoubleInput("Enter your daily calorie goal: ", 500, 10000);
        userProfile->setFixedCalorieGoal(fixedGoal);
    }
    
    // Save the profile changes
    if (dietManager->saveUserProfile()) {
        // Display the new target calories
        double newTargetCalories = userProfile->getTargetCalories(currentDate);
        std::cout << "\nTarget calorie method updated successfully!" << std::endl;
        std::cout << "New target calories: " << std::fixed << std::setprecision(1) << newTargetCalories << std::endl;
    } else {
        std::cout << "\nFailed to save profile changes." << std::endl;
    }
}

void UserInterface::handleSetDate(const std::vector<std::string>& args) {
    std::cout << "\nSet Current Date\n" << SEPARATOR << std::endl;
    
    // Display current date
    std::cout << "Current date: " << currentDate.toString() << std::endl;
    
    Date newDate;
    
    if (args.empty()) {
        // Ask for a date
        newDate = getDateInput("Enter new date (YYYY-MM-DD or 'today' for current date): ");
    } else {
        // Parse the date from the argument
        std::string dateStr = args[0];
        
        if (dateStr == "today") {
            newDate = Date::today();
        } else {
            try {
                newDate = Date(dateStr);
            } catch (const std::exception& e) {
                std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
                return;
            }
        }
    }
    
    // Set the new date
    currentDate = newDate;
    std::cout << "Current date set to: " << currentDate.toString() << std::endl;
}

Date UserInterface::getDateInput(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "today") {
            return Date::today();
        }
        
        // Try to parse the date
        try {
            Date date(input);
            return date;
        } catch (const std::exception& e) {
            std::cout << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
        }
    }
}

void UserInterface::handleUndo() {
    if (dietManager->getCommandManager()->canUndo()) {
        if (dietManager->getCommandManager()->undo()) {
            std::cout << "Command undone successfully." << std::endl;
        } else {
            std::cout << "Failed to undo command." << std::endl;
        }
    } else {
        std::cout << "Nothing to undo." << std::endl;
    }
}

void UserInterface::handleRedo() {
    if (dietManager->getCommandManager()->canRedo()) {
        if (dietManager->getCommandManager()->redo()) {
            std::cout << "Command redone successfully." << std::endl;
        } else {
            std::cout << "Failed to redo command." << std::endl;
        }
    } else {
        std::cout << "Nothing to redo." << std::endl;
    }
}

// Helper method to convert string query to vector of keywords for searching foods
std::vector<FoodPtr> UserInterface::searchFoodsHelper(const std::string& searchQuery, bool matchAll) {
    std::vector<std::string> keywords = parseKeywords(searchQuery);
    return dietManager->getFoodDatabase()->searchFoods(keywords, matchAll);
}
