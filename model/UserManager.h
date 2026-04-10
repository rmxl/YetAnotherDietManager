#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "User.h"
#include <map>
#include <string>
#include <memory>
#include <vector>

/**
 * Manages multiple users and handles authentication
 */
class UserManager {
public:
    UserManager();

    // User management
    bool addUser(const std::string& username, const std::string& password);
    bool userExists(const std::string& username) const;
    UserPtr getUser(const std::string& username) const;
    std::vector<std::string> getAllUsernames() const;
    
    // Authentication
    bool loginUser(const std::string& username, const std::string& password);
    void logoutCurrentUser();
    bool isUserLoggedIn() const;
    UserPtr getCurrentUser() const;
    
    // File operations
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
private:
    std::map<std::string, UserPtr> users;
    UserPtr currentUser;
};

typedef std::shared_ptr<UserManager> UserManagerPtr;

#endif // USER_MANAGER_H
