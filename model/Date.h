#ifndef DATE_H
#define DATE_H

#include <string>
#include <ctime>

/**
 * Represents a date in the system
 */
class Date {
public:
    // Constructors
    Date(); // Current date
    Date(int year, int month, int day);
    Date(const std::string& dateStr); // Format: YYYY-MM-DD
    
    // Accessors
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    
    // Convert to string
    std::string toString() const; // Format: YYYY-MM-DD
    
    // Comparisons
    bool operator==(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
    
    // Static methods
    static Date today();
    static bool isValid(int year, int month, int day);
    
private:
    int year;
    int month;
    int day;
};

#endif // DATE_H
