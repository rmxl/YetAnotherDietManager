#include "Date.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Date::Date() {
    // Initialize with current date
    time_t t = time(nullptr);
    struct tm* now = localtime(&t);
    
    year = now->tm_year + 1900;
    month = now->tm_mon + 1;
    day = now->tm_mday;
}

Date::Date(int year, int month, int day) {
    if (!isValid(year, month, day)) {
        throw std::invalid_argument("Invalid date");
    }
    
    this->year = year;
    this->month = month;
    this->day = day;
}

Date::Date(const std::string& dateStr) {
    // Parse date string (YYYY-MM-DD)
    std::stringstream ss(dateStr);
    char delimiter;
    
    ss >> year >> delimiter >> month >> delimiter >> day;
    
    if (ss.fail() || !isValid(year, month, day)) {
        throw std::invalid_argument("Invalid date string format: " + dateStr);
    }
}

int Date::getYear() const {
    return year;
}

int Date::getMonth() const {
    return month;
}

int Date::getDay() const {
    return day;
}

std::string Date::toString() const {
    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << year << "-" 
       << std::setw(2) << month << "-" 
       << std::setw(2) << day;
    return ss.str();
}

bool Date::operator==(const Date& other) const {
    return year == other.year && month == other.month && day == other.day;
}

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator<=(const Date& other) const {
    return *this < other || *this == other;
}

bool Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const {
    return !(*this < other);
}

Date Date::today() {
    return Date();
}

bool Date::isValid(int year, int month, int day) {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    
    // Days in month
    const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Adjust for leap year
    int maxDays = daysInMonth[month];
    if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
        maxDays = 29;
    }
    
    return day > 0 && day <= maxDays;
}
