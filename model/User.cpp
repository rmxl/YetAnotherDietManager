#include "User.h"
#include <functional> // for std::hash
#include <sstream>
#include <iomanip>

User::User() : loggedIn(false) {}

User::User(const std::string& username, const std::string& passwordHash)
    : username(username), passwordHash(passwordHash), loggedIn(false) {}

std::string User::getUsername() const {
    return username;
}

std::string User::getPasswordHash() const {
    return passwordHash;
}

bool User::isLoggedIn() const {
    return loggedIn;
}

bool User::login(const std::string& password) {
    if (verifyPassword(password)) {
        loggedIn = true;
        return true;
    }
    return false;
}

void User::logout() {
    loggedIn = false;
}

std::string User::hashPassword(const std::string& password) {
    // Simple hash function using std::hash
    // In a real application, use a proper cryptographic hash function like bcrypt
    size_t hash = std::hash<std::string>{}(password);
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

bool User::verifyPassword(const std::string& password) const {
    return passwordHash == hashPassword(password);
}
