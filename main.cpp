#include <iostream>
#include <string>
#include <filesystem>
#include "controller/DietManager.h"
#include "view/UserInterface.h"

/**
 * YADA - Yet Another Diet Assistant
 * Main entry point for the application
 */
int main() {
    // Create data directory if it doesn't exist
    std::string dataPath = "./data";
    std::filesystem::create_directories(dataPath);
    
    // Initialize the diet manager
    auto dietManager = std::make_shared<DietManager>();
    
    if (!dietManager->initialize(dataPath)) {
        std::cerr << "Failed to initialize the application." << std::endl;
        return 1;
    }
    
    // Create and run the user interface
    UserInterface ui(dietManager);
    ui.run();
    
    return 0;
}
