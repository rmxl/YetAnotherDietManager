#ifndef USER_H
#define USER_H

#include <string>
#include <memory>

/**
 * Represents a user account for authentication
 */
class User {
public:
    User();
    User(const std::string& username, const std::string& passwordHash);
    
    // Getters
    std::string getUsername() const;
    std::string getPasswordHash() const;
    bool isLoggedIn() const;
    
    // Authentication methods
    bool login(const std::string& password);
    void logout();
    
    // Utility methods
    static std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password) const;
    
private:
    std::string username;
    std::string passwordHash;
    bool loggedIn;
};

typedef std::shared_ptr<User> UserPtr;

#endif // USER_H
