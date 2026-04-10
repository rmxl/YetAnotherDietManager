#include "UserManager.h"
#include "../util/FileHandler.h"
#include "../util/StringUtils.h"
#include <fstream>
#include <iostream>

UserManager::UserManager() : currentUser(nullptr) {}

bool UserManager::addUser(const std::string& username, const std::string& password) {
    // Check if user already exists
    if (userExists(username)) {
        return false;
    }
    
    // Create new user with hashed password
    std::string passwordHash = User::hashPassword(password);
    UserPtr newUser = std::make_shared<User>(username, passwordHash);
    users[username] = newUser;
    
    return true;
}

bool UserManager::userExists(const std::string& username) const {
    return users.find(username) != users.end();
}

UserPtr UserManager::getUser(const std::string& username) const {
    auto it = users.find(username);
    if (it != users.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> UserManager::getAllUsernames() const {
    std::vector<std::string> usernames;
    for (const auto& pair : users) {
        usernames.push_back(pair.first);
    }
    return usernames;
}

bool UserManager::loginUser(const std::string& username, const std::string& password) {
    UserPtr user = getUser(username);
    if (user && user->login(password)) {
        currentUser = user;
        return true;
    }
    return false;
}

void UserManager::logoutCurrentUser() {
    if (currentUser) {
        currentUser->logout();
        currentUser = nullptr;
    }
}

bool UserManager::isUserLoggedIn() const {
    return currentUser != nullptr && currentUser->isLoggedIn();
}

UserPtr UserManager::getCurrentUser() const {
    return currentUser;
}

bool UserManager::loadFromFile(const std::string& filename) {
    std::vector<std::string> lines;
    if (!FileHandler::readLines(filename, lines)) {
        // If file doesn't exist, that's ok - we're starting with a clean slate
        return true;
    }
    
    for (const auto& line : lines) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }
        
        std::vector<std::string> parts = StringUtils::split(line, '|');
        if (parts.size() >= 2) {
            std::string username = parts[0];
            std::string passwordHash = parts[1];
            
            UserPtr user = std::make_shared<User>(username, passwordHash);
            users[username] = user;
        }
    }
    
    return true;
}

bool UserManager::saveToFile(const std::string& filename) const {
    std::vector<std::string> lines;
    
    lines.push_back("# YADA User Accounts");
    lines.push_back("# Format: username|passwordHash");
    
    for (const auto& pair : users) {
        const UserPtr& user = pair.second;
        std::string line = user->getUsername() + "|" + user->getPasswordHash();
        lines.push_back(line);
    }
    
    return FileHandler::writeLines(filename, lines);
}
